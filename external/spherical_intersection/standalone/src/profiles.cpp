#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include "input_parser.h"
#include "obj.h"
#include "ply.h"
#include "algorithm/component_count.h"
#include "algorithm/sphere_surface_msii.h"
#include "algorithm/sphere_volume_msii.h"
#include "algorithm/sphere_intersections_msii.h"
#include "graph.h"
#include "mesh.h"
#include "timer.h"

namespace {
bool help_is_requested = false;

std::string input_path;
bool input_path_is_set = false;

enum File_Format { obj, ply };
File_Format input_format;
bool input_format_is_set = false;

double radius;
bool radius_is_set = false;

std::function<std::vector<double>(spherical_intersection::Graph &)> algorithm;
bool algorithm_is_set = false;

std::string output_path;
bool output_path_is_set = false;

std::size_t thread_count;
bool thread_count_is_set = false;

std::size_t max_thread_load;
bool max_thread_load_is_set = false;

void set_parser_up(Input_Parser &input_parser) {
	input_parser.add_flag(help_is_requested, "-help", "Displays help.");

	input_parser.add_value(input_path, "-input", "Sets the input path.",
			       &input_path_is_set);

	std::function<File_Format(const std::string &format_name)>
	    format_conversion =
		[](const std::string &format_name) -> File_Format {
		if (format_name == "obj") {
			return File_Format::obj;
		} else if (format_name == "ply") {
			return File_Format::ply;
		} else {
			throw std::invalid_argument(
			    "Unknown file format given.");
		}
	};

	input_parser.add_value(
	    input_format, "-format",
	    "Sets the input format. Possible values: obj, ply",
	    &input_format_is_set, format_conversion);

	input_parser.add_value(radius, "-radius", "Sets the sphere radius.",
			       &radius_is_set);

	std::function<std::function<std::vector<double>(spherical_intersection::Graph &)>(
	    const std::string &string)>
	    algorithm_conversion = [](const std::string &algorithm_name)
	    -> std::function<std::vector<double>(spherical_intersection::Graph &)> {
		if (algorithm_name == "sphere_intersections") {
			return
			    [](spherical_intersection::Graph &graph) -> std::vector<double> {
				    return spherical_intersection::algorithm::
					get_sphere_intersections(graph);
			    };
		} else {
			throw std::invalid_argument("Unknown algorithm given.");
		}
	};

	input_parser.add_value(algorithm, "-algorithm",
			       "Sets the algorithm. Possible values: "
			       "sphere_intersections",
			       &algorithm_is_set, algorithm_conversion);

	input_parser.add_value(
	    output_path, "-output",
	    "Sets the output path for the generated .ply file.",
	    &output_path_is_set);

	input_parser.add_value(thread_count, "-threads",
			       "Sets the number of threads to be used.",
			       &thread_count_is_set);

	input_parser.add_value(
	    max_thread_load, "-max_load",
	    "Sets the maximum number of values to be calculated in each thread "
	    "without notification.",
	    &max_thread_load_is_set);
}

bool arguments_are_valid() {
	bool result = true;
	if (!input_path_is_set) {
		std::cout << "Error: No input path given. Use -help for help."
			  << std::endl;
		result = false;
	}
	if (!input_format_is_set) {
		std::cout << "Error: No file format given. Use -help for help."
			  << std::endl;
		result = false;
	}
	if (!radius_is_set) {
		std::cout << "Error: No radius given. Use -help for help."
			  << std::endl;
		result = false;
	}
	if (!algorithm_is_set) {
		std::cout << "Error: No algorithm given. Use -help for help."
			  << std::endl;
		result = false;
	}
	if (!output_path_is_set) {
		std::cout << "Error: No output path given. Use -help for help."
			  << std::endl;
		result = false;
	}
	if (!thread_count_is_set) {
		std::cout << "Error: No thread count given. Use -help for help."
			  << std::endl;
		result = false;
	}
	if (!max_thread_load_is_set) {
		std::cout
		    << "Error: No maximum number of values to be calculated in "
		       "each thread given. Use -help for help."
		    << std::endl;
		result = false;
	}
	return result;
}

std::vector<std::vector<double>> compute_all_values(
    const spherical_intersection::Mesh &mesh, double radius,
    std::function<std::vector<double>(spherical_intersection::Graph &graph)> algorithm,
    std::size_t thread_count) {
	const auto &vertices = mesh.get_vertices();
	std::size_t vertex_count = vertices.size();
	std::vector<std::vector<double>> values(vertex_count);
	auto set_range = [&vertices, &values, &algorithm, &radius](
			     std::size_t start_index, std::size_t amount) {
		for (std::size_t index = start_index;
		     index < start_index + amount; index++) {
			const auto &vertex = vertices[index];
			spherical_intersection::math3d::Sphere sphere{
			    vertex.get_location(), radius};
			spherical_intersection::Graph graph{vertices[index],
							    sphere};
			values[index] = algorithm(graph);
		}
	};

	std::size_t current_start_index = 0;
	std::size_t load_per_thread = max_thread_load;
	std::cout << std::endl;
	while (thread_count * load_per_thread <
	       vertex_count - current_start_index) {
		std::vector<std::thread> threads;
		for (std::size_t thread_idx = 0; thread_idx < thread_count;
		     thread_idx++) {
			threads.emplace_back(set_range, current_start_index,
					     load_per_thread);
			current_start_index += load_per_thread;
		}
		for (auto &thread : threads) {
			thread.join();
		}
		std::cout << (100.0 * current_start_index) / vertex_count << "%"
			  << std::endl;
	}

	std::vector<std::thread> threads;
	load_per_thread = (vertex_count - current_start_index) / thread_count;
	std::size_t load_for_last_thread =
	    ((vertex_count - current_start_index) / thread_count) +
	    ((vertex_count - current_start_index) % thread_count);
	for (std::size_t thread_idx = 0; thread_idx < thread_count - 1;
	     thread_idx++) {
		threads.emplace_back(set_range, current_start_index,
				     load_per_thread);
		current_start_index += load_per_thread;
	}
	threads.emplace_back(set_range, current_start_index,
			     load_for_last_thread);
	for (auto &thread : threads) {
		thread.join();
	}
	std::cout << "100%" << std::endl;
	return values;
}
} // namespace


bool save_features(std::string output_path, std::vector<std::vector<double>> &features){
	std::ofstream file;
	file.open(output_path);

	for (int numbOfVertices = 0;numbOfVertices<features.size();numbOfVertices++)
	{
		for (int numbOfFeatures = 0; numbOfFeatures != features[numbOfVertices].size(); numbOfFeatures+=3)
		{

			file << numbOfVertices << " " << numbOfFeatures/3 << " " << features[numbOfVertices][0+numbOfFeatures] << " "
			     << features[numbOfVertices][1+numbOfFeatures] << " " << features[numbOfVertices][2+numbOfFeatures] << "\n";
		}
	}

	file.close();
	return true;
}


int main(int argc, char *argv[]) {
	Input_Parser input_parser;
	set_parser_up(input_parser);
	input_parser.parse(argc, argv);
	bool args_are_valid = arguments_are_valid();

	if (help_is_requested) {
		std::cout << input_parser.get_help() << std::endl;
		std::cout << "Example shell command:" << std::endl;
		std::cout << "./profiles -format obj -algorithm sphere_intersections -max_load 100 -radius 1 -threads 2 " << std::endl;
		std::cout << "-input /export/home/mspankus/Documents/project/scripts/Data/sample1.obj " << std::endl;
		std::cout << "-output /export/home/mspankus/Documents/project/scripts/Data/processedSample1.obj  " << std::endl;
	}

	if (!args_are_valid) {
		return 0;
	}

	spherical_intersection::Mesh mesh;

	std::cout << "Loading mesh... " << std::flush;
	auto object_information = [&] {
		switch (input_format) {
		case obj:
			return object_io::obj::load_obj(input_path);
		case ply:
			return object_io::ply::load_ply(input_path);
		default:
			throw std::invalid_argument("Unknown input format.");
		}
	}();
	std::cout << "Done!" << std::endl;

	std::cout << "Building mesh... " << std::flush;
	for (const auto &vertex_location : object_information.first) {
		mesh.add_vertex(spherical_intersection::math3d::Vector{
		    vertex_location[0], vertex_location[1],
		    vertex_location[2]});
	}
	for (const auto &triangle_indices : object_information.second) {
		const auto &vertices = mesh.get_vertices();
		mesh.add_triangle(vertices[triangle_indices[0]],
				  vertices[triangle_indices[1]],
				  vertices[triangle_indices[2]]);
	}
	std::cout << "Done!" << std::endl;

	std::cout << "Computing values... " << std::flush;
	timer::Timer::start("computation");
	auto values = compute_all_values(mesh, radius, algorithm, thread_count);
	timer::Timer::stop("computation");
	std::cout << "Done!" << std::endl;

	std::cout << "Computation speed: "
		  << values.size() / timer::Timer::get("computation")
		  << " vertices per second" << std::endl;

	std::cout << "Exporting values... " << std::flush;
	bool saveSuccess = save_features(output_path,values);
	std::cout << "Done!" << std::endl;

	return 0;
}
