# ###############################################################################
# MANUALLY generated by Hubert Mara
# ###############################################################################
# getting started?
# than consider: http://doc.qt.io/qt-5/qmake-advanced-usage.html
# ===============================================================================
# Windows Build: (older info)
# -------------------------------------------------------------------------------
# For libtiff you need three libs: jpeg62.dll, libtiff3.dll and zlib1.dll
# You can find them here: http://www.dlldump.com/
# Additonally you may need to copy the .dll files from C:\Qt\2010.02\qt\bin
# into the same path as gigamesh.exe
# ===============================================================================
# Ubuntu 18.04 LTS build with gcc in version 7 as default:
# add
#     CC=gcc-8 CXX=g++-8 LINK=g++-8
# as extra make options!
# ===============================================================================

TEMPLATE = app
TARGET = gigamesh

TRANSLATIONS = languages/GigaMesh_de.ts \
			   languages/GigaMesh_ja.ts \
			   languages/GigaMesh_en.ts

# -------------------------------------------------------------------------------
# Git info:
# -------------------------------------------------------------------------------
COMPGITREPO=$$system(git rev-parse --is-inside-work-tree)

equals (COMPGITREPO, true) {
    COMPGITCOMMIT= $$system(git rev-parse HEAD)
    DEFINES +=COMP_GITHEAD=\\\"$$COMPGITCOMMIT\\\"
    message( Build based on git commit $$COMPGITCOMMIT )
}
else: {
    message( Build is NOT from git. )
}

# -------------------------------------------------------------------------------
# See: http://doc.qt.io/qt-5/qmake-variable-reference.html#version
# -------------------------------------------------------------------------------
# We use 2 digits: year.month.day instead of an incremental numbering
#VERSION = $$system( git log -1 --format=%ci | cut -b3-10 | tr "-" "." )
# Command used by the packing: git log -1 --format=%ci | cut -b3,4,6,7,9,10
#GITCOMITDATE = $$system("git log -1 --format=%ci | cut -b3,4,6,7,9,10")

GITLOG = $$system( git log -1 --format=%ci)
VERSION = $$str_member($$replace(GITLOG, "-" , "."), 2 , 9)

GITCOMITDATE = $$replace(VERSION, "\\." , "")
DEFINES += VERSION_PACKAGE=\\\"$$GITCOMITDATE\\\"
# -------------------------------------------------------------------------------

exists( "/tmp/qtobjects.$$USERNAME" ) {
    OBJECTS_DIR = /tmp/qtobjects.$$USERNAME
}
message( USING OBJECTS DIR: $$OBJECTS_DIR )

# -------------------------------------------------------------------------------
# In case QT5 kit is used, Widgets module needs to be loaded explicitly

message(Qt version: $$[QT_VERSION])
lessThan( QT_MAJOR_VERSION, 5 ) {
    error( Qt Version of 5 or greater is required! )
}
lessThan( QT_MINOR_VERSION, 2 ) {
    error( Qt Version greater than 5.2 required! )
}
# User interface with OpenGL
QT += widgets opengl gui
# Add the Network-Manager
QT += network networkauth

# -------------------------------------------------------------------------------

# -------------------------------------------------------------------------------
# Custom version info:
# -------------------------------------------------------------------------------
#VER_EDITION = "'IWR.build: AO Jena'"   # Win32
#VER_EDITION = "IWR.build:\\ AO\\ Jena" # Linux
#VER_EDITION = "Uni\\ Wurzburg"
#VER_EDITION = "IWR\\ MAA\\ D.W."
#VER_EDITION = "IWR\\ PHd\\ CCK"
#VER_EDITION = "IWR.build:\\ Vis\\ Koeln"
#VER_EDITION = "IWR.build:\\ GWZ\\ Leipzig"
#VER_EDITION = "IWR.build:\\ Y.Y."
# -------------------------------------------------------------------------------

# to enable public version: qmake VER_EDITION="\"Someplace\ University\""
isEmpty( VER_EDITION ) {
        message( INTERNAL BUILD )
        unix: VER_EDITION = "IWR\\ FreeWare"
        win32:VER_EDITION = "'IWR_FreeWare'"
#	unix: VER_EDITION = "IWR\\ Internal"
#	win32:VER_EDITION = "'IWR Internal'"
} else: {
        message( PUBLIC BUILD for: $$VER_EDITION )
        unix: {
                QMAKE_POST_LINK += strip gigamesh_public
                TARGET = gigamesh_public
        }
        DEFINES += GIGAMESH_PUBLIC_METHODS_ONLY=\"true\"
}

# -------------------------------------------------------------------------------
# Platform specific settings:
# -------------------------------------------------------------------------------
unix {
    USERNAME = $$system(whoami)          # http://doc.qt.io/qt-5/qmake-variable-reference.html#version
    COMPNAME = $$system(hostname -s)
    COMPILEDATE = $$system(date +%d.%b.%Y)
    CONFIG  += thread
    DEFINES += THREADS

    QMAKE_CXXFLAGS += -std=c++17 -Wextra -Wpedantic -Wl,-z,relro
    QMAKE_CXXFLAGS += -Werror=format-security -pedantic-errors
    QMAKE_CXXFLAGS += -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables
    QMAKE_CXXFLAGS += -fexceptions -fpie -Wl,-pie -fstack-protector-strong
    QMAKE_CXXFLAGS_RELEASE += -D_FORTIFY_SOURCE=2

    # SANITIZER
    #    QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -fsanitize=undefined
    #    QMAKE_LFLAGS_DEBUG += -fsanitize=address -fsanitize=undefined

    LIBS += -lstdc++fs

    macx {
        ICON = GigaMesh_Logo.icns
        QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=gnu17 -stdlib=libc+
        CONFIG += c++17

        # this block has to be (re)tested!
        INCLUDEPATH += /usr/local/include/QtOpenGL/ #using homebrew!
        INCLUDEPATH +=  /usr/local/Cellar/ #also Homebrew
        #INCLUDEPATH += /Library/Frameworks/QtOpenGL.framework/Headers
        #LIBS += -lQtOpenGL
        #LIBS += -framework lGLULIBS
        LIBS += -framework QtOpenGL
        LIBS += -framework Foundation
        LIBS += -framework AGL
        LIBS += -framework GLUT
        LIBS += -framework OpenGL
        LIBS += -framework Cocoa
        DEFINES += MACXPORT
    }
}

win32 {
    RC_ICONS = forms/GigaMesh_Logo.ico
    LIBS += -lws2_32

    win32-msvc {
        USERNAME = $$system(echo %USERNAME%) # whoami works too, but it returns "computer\user" and the "\" causes troubles.
        COMPNAME = $$system(hostname)
        COMPILEDATE = $$system(date /t)

        DEFINES += THREADS
        #needed for triangle.c to avoid inclusion of sys/time
        DEFINES += NO_TIMER
        #avoid using min/max from windows.h
        DEFINES += NOMINMAX
        CONFIG += console

        QMAKE_CXXFLAGS += /std:c++latest /permissive-

        LIBS += -lOpenGL32 -lAdvapi32
    }

    win32-g++ {
        USERNAME = $$system(whoami)
        COMPNAME = $$system(hostname -s)
        COMPILEDATE = $$system(date +%d.%b.%Y)

        CONFIG  += thread
        DEFINES += THREADS
        CONFIG += console

		QMAKE_CXXFLAGS += -std=c++17 -Wextra -Wpedantic -Wl,-z,relro
		QMAKE_CXXFLAGS += -Werror=format-security -pedantic-errors
		QMAKE_CXXFLAGS += -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables
		QMAKE_CXXFLAGS += -fexceptions -fpie -Wl,-pie
		QMAKE_CXXFLAGS_RELEASE += -D_FORTIFY_SOURCE=2
        QTPLUGIN += qsvgicon qsvg
    }
}


# -------------------------------------------------------------------------------

DEFINES += COMP_USER=\\\"$$USERNAME@$$COMPNAME\\\"
DEFINES += COMP_DATE=\\\"$$COMPILEDATE\\\"
DEFINES += COMP_EDIT=\\\"$$VER_EDITION\\\"

# From first version - see commented section in qruntpsrpmscriptdialog.cpp:233
#DEFINES += TPS_RPM_SCRIPT_FILENAME=\\\"gigamesh_warp_vertices.py\\\"
#TPS_RPM_SCRIPT_FILE_HASH_INTERNAL = $$system("sha512sum gigamesh_warp_vertices.py | cut -d \" \" -f 1")
#DEFINES += TPS_RPM_SCRIPT_FILE_HASH=\\\"$$TPS_RPM_SCRIPT_FILE_HASH_INTERNAL\\\"
#message(TPS-RPM python script hash: $$TPS_RPM_SCRIPT_FILE_HASH_INTERNAL)

CONFIG( thread ) { # General
        message( Multithreading active. )
} else {
        message( No multithreading added! )
}

message( --- Libraries ----------------------------------------- )

# --- TIFF ---------------------------------------------------------------------
# 1a. try including shared library (Windows)
win32:exists( $(TIFF)/include/tiffio.h ) { # NOT TESTED!!!
    CONFIG      += tiff
    # LIBS        += -ltiff -L"%TIFF%/lib/"
    # INCLUDEPATH += "%TIFF%/include/"
    LIBS        += -ltiff
    LIBS        += -L$(TIFF)/lib
    INCLUDEPATH += "$(TIFF)/include/"
}
# 1b. try including shared library (Linux)
unix:!macx:exists( "/usr/include/tiff.h" ) { # older systems e.g. Ubuntu <= 13.04
    CONFIG += tiff
    LIBS   += -ltiff
}
unix:!macx:exists( "/usr/include/x86_64-linux-gnu/tiff.h" ) { # newer systems e.g. Ubuntu >= 13.10
    CONFIG += tiff
    LIBS   += -ltiff
}
# 1c. try including shared library (MacOS)
macx:exists( "/usr/local/opt/tiff.h" ) {
    INCLUDEPATH += "/opt/local/include"
    INCLUDEPATH += "/usr/local/lib"
    CONFIG      += tiff
    LIBS        += -ltiff
}
# 2. Include headers and set define, when libtiff was found.
CONFIG( tiff ) {
    DEFINES     += LIBTIFF
    message( LIBTIFF (required, shared) present. )
} else {
    message( LIBTIFF (required) not present. Install Ubuntu package: libtiff5-dev )
}
# -------------------------------------------------------------------------------

# --- CUDA ----------------------------------------------------------------------
# INCLUDEPATH += /usr/include/cuda/
# LIBS        += -lcuda -lcudart -L/usr/local/cuda/lib  # generic/offical  CUDA libraries used by:
# LIBS        += -lcudamesh -Lcuda                      # home-made CUDA  library files ;)
# INCLUDEPATH += ./cuda                                 # ... and their  header location
# -------------------------------------------------------------------------------

# --- SPHERICAL_INTERSECTION LIB ------------------------------------------------------------------
exists( "external/spherical_intersection/libspherical_intersection.a" ) {
    DEFINES     += LIBSPHERICAL_INTERSECTION
	INCLUDEPATH += $$PWD/external/spherical_intersection/include
	LIBS        += -lspherical_intersection -L$$PWD/external/spherical_intersection
    message( LIBSPHERICAL_INTERSECTION present. )
}
else:message( ERROR: LIBSPHERICAL_INTERSECTION (optional) not present!)
# -------------------------------------------------------------------------------

# --- PSALM LIB ------------------------------------------------------------------
win32-msvc {
exists( "external/libpsalmBoostless/psalm.lib" ) {
    # to build libpsalm, the packages: cmake and libboostX.XX-all-dev
    # its on branch libpsalm
        DEFINES     += LIBPSALM
        INCLUDEPATH += $$PWD/external/libpsalmBoostless/include                   # subdirectory with psalm - git submodul
        LIBS        += -lpsalm -L$$PWD/external/libpsalmBoostless         # add as lib including its directory
message( LIBPSALM present. )
}
else:message( ERROR: LIBPSALM (optional) not present! see: README_LIBPSALM.txt )
}

else {
exists( "external/libpsalmBoostless/libpsalm.a" ) {
    # to build libpsalm, the packages: cmake and libboostX.XX-all-dev
    # its on branch libpsalm
        DEFINES     += LIBPSALM
        INCLUDEPATH += $$PWD/external/libpsalmBoostless/include                   # subdirectory with psalm - git submodul
        LIBS        += -lpsalm -L$$PWD/external/libpsalmBoostless         # add as lib including its directory
message( LIBPSALM present. )
}
else:message( ERROR: LIBPSALM (optional) not present! see: README_LIBPSALM.txt )
}
#-------------------------------------------------------------------------------

# --- ALGlib --------------------------------------------------------------------
# from http://www.alglib.net
# -------------------------------------------------------------------------------
win32-msvc {
    exists( "external\alglib\out\libalglib.lib" ) {
        DEFINES     += ALGLIB
        INCLUDEPATH += $$PWD\external\alglib\src    # Header files
        LIBS        += -llibalglib
        LIBS        += -L$$PWD\external\alglib\out  # Static library and library path
        message( ALGLIB present. )
    } else {
        message( WARNING: ALGLIB (optional) not present! )
    }
}

else {
    exists( "external/alglib/out/libalglib.a" ) {
        DEFINES     += ALGLIB
        INCLUDEPATH += $$PWD/external/alglib/src    # Header files
        LIBS        += -lalglib
        LIBS        += -L$$PWD/external/alglib/out  # Static library and library path
        message( ALGLIB present. )
    } else {
        message( WARNING: ALGLIB (optional) not present! )
    }
}
# -------------------------------------------------------------------------------

# ---- Resources ----------------------------------------------------------------
RESOURCES += forms/gigamesh.qrc
RESOURCES += languages/translations.qrc
RESOURCES += python-scripts/pythonscripts.qrc
RESOURCES += latex_report_template/latex_templates.qrc
RESOURCES += shaders/shaders.qrc

# -------------------------------------------------------------------------------
# Forms created using the UI-Designer:
# -------------------------------------------------------------------------------
FORMS += forms/mainWin.ui \
    forms/qruntpsrpmscriptdialog.ui \
    forms/ExternalProgramsDialog.ui \
    forms/dialogGridCenterSelect.ui \
    normalSphereSelection/NormalSphereSelectionDialog.ui
FORMS += forms/qgmdialognprsettings.ui
FORMS += forms/QGMDialogTransparencySettings.ui
FORMS += forms/QGMDialogSliderHD.ui
FORMS += forms/qgmdocksidebar.ui
FORMS += forms/qgmdockinfo.ui
FORMS += forms/qgmdockview.ui

FORMS += forms/dialogAbout.ui
FORMS += forms/dialogConeParam.ui
FORMS += forms/dialogMSII.ui
FORMS += forms/dialogCutOffValues.ui
FORMS += forms/dialogEnterText.ui
FORMS += forms/dialogPlaneParam.ui
FORMS += forms/dialogComboBox.ui
FORMS += forms/dialogRuler.ui

# -------------------------------------------------------------------------------
DEPENDPATH += .

# --- Multithreading using pthreads ---------------------------------------------


message( ------------------------------------------------------- )

# -------------------------------------------------------------------------------
# our own internal data structure;
# -------------------------------------------------------------------------------
DEPENDPATH += ./voxel   # Layer 0        for Volume data
DEPENDPATH += ./mesh    # Layer 0 and -1 for Mesh data
DEPENDPATH += ./meshGL  # Layer 1        for Mesh data
INCLUDEPATH += ./voxel  # Layer 0        for Volume data
INCLUDEPATH += ./mesh   # Layer 0 and -1 for Mesh data
INCLUDEPATH += ./meshGL # Layer 1        for Mesh data
VPATH += ./voxel ./mesh ./meshGL
# -------------------------------------------------------------------------------
# Layer -1: specialized classes for Layer 0 classes
# -------------------------------------------------------------------------------
# Bit array for flags
HEADERS += meshGL/meshGLShader.h \
        ExternalProgramsDialog.h \
	logging/Logger.h \
	logging/Logging.h \
	mesh/MeshIO/MeshReader.h \
	mesh/MeshIO/MeshWriter.h \
	mesh/MeshIO/ModelMetaData.h \
	mesh/MeshIO/MtlParser.h \
	mesh/MeshIO/ObjReader.h \
	mesh/MeshIO/ObjWriter.h \
	mesh/MeshIO/PlyEnums.h \
	mesh/MeshIO/PlyReader.h \
	mesh/MeshIO/PlyWriter.h \
	mesh/MeshIO/RegularGridTxtReader.h \
	mesh/MeshIO/TxtReader.h \
	mesh/MeshIO/TxtWriter.h \
	mesh/MeshIO/VRMLWriter.h \
	meshGL/PinGeometry.h \
	meshGL/TexturedMesh.h \
	meshGL/TexturedMeshRenderer.h \
    meshGL/meshGL_params.h \
    mesh/triangle/triangleTriangleIntersection.h \
    mesh/ellipsedisc.h \
    meshGL/meshglcolors.h \
    QGMDialogNprSettings.h \
    QGMDialogTransparencySettings.h \
        meshGL/PinRenderer.h \
	normalSphereSelection/ArcBall.h \
	normalSphereSelection/IcoSphereTree.h \
	normalSphereSelection/NormalSphereSelectionDialog.h \
	normalSphereSelection/NormalSphereSelectionRenderWidget.h \
    svg/SvgWriter.h \
    svg/SvgElement.h \
    svg/SvgPath.h \
    svg/SvgImage.h \
    svg/SvgText.h \
    svg/SvgIncludes.h \
    svg/SvgRect.h \
    svg/SvgCircle.h
HEADERS += MeshQtCSVImportExport.h \
    qruntpsrpmscriptdialog.h \
    mesh/showprogress.h \
    mesh/userinteraction.h
HEADERS += mesh/meshinfodata.h
SOURCES += mesh/meshinfodata.cpp \
        ExternalProgramsDialog.cpp \
    MeshQtCSVImportExport.cpp \
	logging/Logger.cpp \
	logging/Logging.cpp \
	mesh/MeshIO/MeshReader.cpp \
	mesh/MeshIO/MeshWriter.cpp \
	mesh/MeshIO/ModelMetaData.cpp \
	mesh/MeshIO/MtlParser.cpp \
	mesh/MeshIO/ObjReader.cpp \
	mesh/MeshIO/ObjWriter.cpp \
	mesh/MeshIO/PlyReader.cpp \
	mesh/MeshIO/PlyWriter.cpp \
	mesh/MeshIO/RegularGridTxtReader.cpp \
	mesh/MeshIO/TxtReader.cpp \
	mesh/MeshIO/TxtWriter.cpp \
	mesh/MeshIO/VRMLWriter.cpp \
        meshGL/PinRenderer.cpp \
	meshGL/TexturedMesh.cpp \
	meshGL/TexturedMeshRenderer.cpp \
	normalSphereSelection/ArcBall.cpp \
	normalSphereSelection/IcoSphereTree.cpp \
	normalSphereSelection/NormalSphereSelectionDialog.cpp \
	normalSphereSelection/NormalSphereSelectionRenderWidget.cpp \
    svg/SvgWriter.cpp \
    svg/SvgPath.cpp \
    svg/SvgImage.cpp \
    svg/SvgText.cpp \
    svg/SvgRect.cpp \
    svg/SvgCircle.cpp
HEADERS += mesh/bitflagarray.h \
    mesh/triangle/triangle.h \
    mesh/line.h \
    mesh/vertexofface.h \
    qgmdocksidebar.h \
    qgmdockinfo.h \
    mesh/cube.h \
    mesh/triangularprism.h
SOURCES +=     meshGL/meshGLShader.cpp \
    meshGL/meshGL_params.cpp \
    mesh/triangle/triangleTriangleIntersection.cpp \
    mesh/ellipsedisc.cpp \
    meshGL/meshglcolors.cpp \
    QGMDialogNprSettings.cpp \
        QGMDialogTransparencySettings.cpp \
    qruntpsrpmscriptdialog.cpp \
    mesh/showprogress.cpp \
    mesh/userinteraction.cpp
SOURCES += mesh/bitflagarray.cpp \
    mesh/triangle/triangle.c \
    mesh/line.cpp \
    mesh/vertexofface.cpp \
    qgmdocksidebar.cpp \
    qgmdockinfo.cpp \
    mesh/cube.cpp \
    mesh/triangularprism.cpp
HEADERS += qgmdockview.h
SOURCES += qgmdockview.cpp
# marching front - quite outdated:
HEADERS += marchingfront.h
SOURCES += marchingfront.cpp
# Octree
HEADERS     += mesh/octnode.h # There exists no octnode.cpp, because template classes have to be implemented in tje header files using inline functions.
HEADERS     += mesh/octree.h
OTHER_FILES += mesh/octree.cpp # Can not be included using "SOURCES" because of the template class.

# -------------------------------------------------------------------------------
# Layer 0: Executeables for testing
# -------------------------------------------------------------------------------

OTHER_FILES += mesh/matrix4d_main.c
OTHER_FILES += mesh/vector3d_main.c

# -------------------------------------------------------------------------------
# Layer 0: our own internal data structure in C++ (no Qt, no OpenGL, no CUDA)
# -------------------------------------------------------------------------------
HEADERS += gmcommon.h

HEADRES += mesh/printbuildinfo.H
SOURCES += mesh/printbuildinfo.cpp

HEADERS += primitive.h
HEADERS += face.h \
    vertex.h
HEADERS += polyline.h
HEADERS += polyedge.h
HEADERS += plane.h
HEADERS += mesh.h \
    meshio.h \
    mesh_params.h \
    heightmappixel.h
HEADERS += meshseedext.h
HEADERS += vector3d.h
HEADERS += matrix4d.h
HEADERS += voxelcuboid.h \
    voxelsphereparam.h
HEADERS += voxelfilter25d.h
HEADERS += image2d.h

SOURCES += primitive.cpp
SOURCES += face.cpp \
    vertex.cpp
SOURCES += polyline.cpp
SOURCES += polyedge.cpp
SOURCES += plane.cpp
SOURCES += mesh.cpp \
    meshio.cpp \
    mesh_params.cpp
SOURCES += meshseedext.cpp
SOURCES += vector3d.cpp
SOURCES += matrix4d.cpp
SOURCES += voxelcuboid.cpp
SOURCES += voxelfilter25d.cpp
SOURCES += image2d.cpp

HEADERS += edgegeodesic.h
SOURCES += edgegeodesic.cpp

HEADERS += geodentry.h
SOURCES += geodentry.cpp

# -------------------------------------------------------------------------------
# Layer 0.5: our own internal data structure in C++ (no Qt, no OpenGL, no CUDA)
# -------------------------------------------------------------------------------
# Datum objects - which serve more visualization aspects (for the moment).
HEADERS += sphere.h
SOURCES += sphere.cpp
HEADERS += rectbox.h
SOURCES += rectbox.cpp

# -------------------------------------------------------------------------------
# Layer 1: openGl extension for our data structure
# -------------------------------------------------------------------------------
HEADERS += glmacros.h
HEADERS += meshGL.h
SOURCES += meshGL.cpp

# -------------------------------------------------------------------------------
# Layer 2: Qt extension for our data structure
# -------------------------------------------------------------------------------
HEADERS += meshQt.h
SOURCES += meshQt.cpp

# -------------------------------------------------------------------------------
# Layer 3: Qt windows
# -------------------------------------------------------------------------------
HEADERS += QGMMainWindow.h
SOURCES += QGMMainWindow.cpp

# --- OpenGL --------------------------------------------------------------------
HEADERS += meshwidget.h \
           meshwidget_params.h
SOURCES += meshwidget.cpp \
           meshwidget_params.cpp

# --- Dialog --------------------------------------------------------------------
HEADERS += QGMDialogMSII.h
SOURCES += QGMDialogMSII.cpp
HEADERS += QGMDialogCutOffValues.h
SOURCES += QGMDialogCutOffValues.cpp
HEADERS += QGMDialogEnterText.h
SOURCES += QGMDialogEnterText.cpp
HEADERS += QGMDialogPlaneParam.h
SOURCES += QGMDialogPlaneParam.cpp
HEADERS += QGMDialogSliderHD.h
SOURCES += QGMDialogSliderHD.cpp
HEADERS += QGMDialogComboBox.h
SOURCES += QGMDialogComboBox.cpp
HEADERS += QGMDialogRuler.h
SOURCES += QGMDialogRuler.cpp
HEADERS += dialogGridCenterSelect.h
SOURCES += dialogGridCenterSelect.cpp

# --- Macros for Qt ------------------------------------------------------------
HEADERS += QGMMacros.h

# ###############################################################################
# INPUT
# ###############################################################################

SOURCES += main.cpp

HEADERS += QGMDialogConeParam.h
SOURCES += QGMDialogConeParam.cpp

HEADERS += QGMDialogAbout.h
SOURCES += QGMDialogAbout.cpp


#--- Shaders GLSL Faces ---------------------------------------------------------
OTHER_FILES += shaders/funcval.vert
OTHER_FILES += shaders/funcval.geom
OTHER_FILES += shaders/funcval.frag
#--- Shaders GLSL sources for the Wireframe -------------------------------------
OTHER_FILES += shaders/wireframe/wireframe.vert
OTHER_FILES += shaders/wireframe/wireframe.geom
OTHER_FILES += shaders/wireframe/wireframe.frag
#--- Shaders GLSL Vertices (Sprites) ---------------------------------------------
OTHER_FILES += shaders/vertex_sprite.frag
OTHER_FILES += shaders/vertex_sprite.geom
OTHER_FILES += shaders/vertex_sprite.vert
#--- Shaders GLSL replacing gluCylinder -------------------------------------------
OTHER_FILES += shaders/cone.frag
OTHER_FILES += shaders/cone.geom
OTHER_FILES += shaders/cone.vert
#--- Shaders GLSL rendering Polylines using GL_LINES and funcval.vert/geom -------
OTHER_FILES += shaders/polyline.geom
#--- Shaders GLSL Bounding Box of a Mesh -----------------------------------------
OTHER_FILES += shaders/boundingbox.vert
OTHER_FILES += shaders/boundingbox.frag
#--- Shaders GLSL sources for the MeshWidget -------------------------------------
OTHER_FILES += shaders/meshwidget_background_grid.vert
OTHER_FILES += shaders/meshwidget_background_grid.frag
OTHER_FILES += shaders/meshwidget_background_polar_lines.vert
OTHER_FILES += shaders/meshwidget_background_polar_lines.frag
OTHER_FILES += shaders/meshwidget_background_polar_circles.vert
OTHER_FILES += shaders/meshwidget_background_polar_circles.frag
OTHER_FILES += shaders/meshwidget_image2d.vert
OTHER_FILES += shaders/meshwidget_image2d.frag
#--- Shaders GLSL sources for Ambient Occlusion ----------------------------------
OTHER_FILES += shaders/depth.frag
OTHER_FILES += shaders/depth.vert
OTHER_FILES += shaders/frontal_light_per_vertex.frag
OTHER_FILES += shaders/frontal_light_per_vertex.vert
#--- Shaders GLSL sources for Transparency ---------------------------------------
OTHER_FILES += shaders/transparency/ABuffer/transparency_minGeom.frag
OTHER_FILES += shaders/transparency/ABuffer/transparency_clearABuffers.frag
OTHER_FILES += shaders/transparency/ABuffer/transparency_createABuffers.frag
OTHER_FILES += shaders/transparency/ABuffer/transparency_minGeom.vert
OTHER_FILES += shaders/transparency/ABuffer/transparency_renderABuffers.frag
OTHER_FILES += shaders/transparency/atomicLoop/transparency_clearALBuffers.frag
OTHER_FILES += shaders/transparency/atomicLoop/transparency_createALBuffers.frag
OTHER_FILES += shaders/transparency/atomicLoop/transparency_renderALBuffers.frag
OTHER_FILES += shaders/transparency/atomicLoop/transparency_ALcollectDepth.frag
OTHER_FILES += shaders/transparency/WeightedOIT/transparency_geomWOIT.frag
OTHER_FILES += shaders/transparency/WeightedOIT/transparency_blendWOIT.frag
OTHER_FILES +=
#--- Shaders GLSL sources for Non-Photorealistic Rendering (NPR) -----------------
OTHER_FILES += shaders/NPR/NPR_ApplySobel.frag
OTHER_FILES += shaders/NPR/NPR_drawFBO.frag
OTHER_FILES += shaders/NPR/NPR_buildFBO.vert
OTHER_FILES += shaders/NPR/NPR_buildFBO.frag
OTHER_FILES += shaders/fullscreenQuad_passthrough.vert
OTHER_FILES += shaders/NPR/NPR_gaussianblur.frag
OTHER_FILES += shaders/NPR/NPR_composit.frag
OTHER_FILES += shaders/NPR/NPR_toonify.frag
OTHER_FILES += shaders/NPR/NPR_hatches.frag
#--- Shaders GLSL sources for Overlay Light / Over- and underexposure ------------
OTHER_FILES += shaders/lightingOverlay/overlayLighting_geometrypass.vert
OTHER_FILES += shaders/lightingOverlay/overlayLighting_geometrypass.frag
OTHER_FILES += shaders/lightingOverlay/overlayLighting_fbopass.frag
#--- Shaders GLSL sources for pointcloud rendering -------------------------------
OTHER_FILES += shaders/pointcloud/pointcloud.geom
OTHER_FILES += shaders/pointcloud/pointcloud.frag
#--- Shaders GLSL sources for pin rendering --------------------------------------
OTHER_FILES += shaders/vertex_pin.vert
OTHER_FILES += shaders/vertex_pin.frag
#--- Shaders GLSL sources for normalSelection rendering --------------------------
OTHER_FILES += shaders/normalSphere/normalData.vert
OTHER_FILES += shaders/normalSphere/normalData.frag

#--- LaTeX Templates ------------------------------------------------------------
OTHER_FILES += forms/latextemplates/test.tex
OTHER_FILES += forms/latextemplates/cross.tex
OTHER_FILES += forms/latextemplates/skeleton.tex
OTHER_FILES += forms/latextemplates/vessols.tex
OTHER_FILES += forms/latextemplates/fatcross+light.tex
OTHER_FILES += forms/latextemplates/fatcross.tex
OTHER_FILES += forms/latextemplates/fatcross-invert.tex
OTHER_FILES += forms/latextemplates/single-view.tex
OTHER_FILES += forms/latextemplates/keydata.tex
OTHER_FILES += forms/latextemplates/manual_report.tex


OTHER_FILES += latex_report_template/figs/setdpi.sh

OTHER_FILES += compare_ftvecs.py

#--- Command line tools ---------------------------------------------------------
OTHER_FILES += mesh/Makefile          # Linux
OTHER_FILES += mesh/Makefile.windows  # Windows
OTHER_FILES += mesh/make_win.bat      # Windows
#--- Command line tools ---------------------------------------------------------
OTHER_FILES += mesh/gigamesh-info.cpp
OTHER_FILES += mesh/gigamesh-clean.cpp
OTHER_FILES += mesh/gigamesh-tolegacy.cpp
OTHER_FILES += mesh/gigamesh-borders.cpp
OTHER_FILES += mesh/meshgeneratefeaturevectors.cpp
OTHER_FILES += mesh/meshgeneratefeaturevectors25d.cpp
OTHER_FILES += mesh/mesh_main.c
OTHER_FILES += voxel/voxelcuboid_main.c

#--- Packaging---- --------------------------------------------------------------
OTHER_FILES += packaging/00_README.txt
OTHER_FILES += packaging/build_newsentry.sh
OTHER_FILES += packaging/build_tarball.sh
OTHER_FILES += packaging/build_deb.sh
OTHER_FILES += packaging/debian/gigamesh.install
OTHER_FILES += packaging/build_manjaro.sh
OTHER_FILES += packaging/arch/PKGBUILD
OTHER_FILES += CHANGELOG

#--- Documentation --------------------------------------------------------------
OTHER_FILES += doxygen.config

#--- Logos ----------------------------------------------------------------------
OTHER_FILES += GigaMesh_Logo.icns      # MacOS Icon embedded in the executeable.
OTHER_FILES += forms/GigaMesh_Logo.ico # Windows Icon embedded in the executeable.

#--- DEPRECATED -----------------------------------------------------------------
OTHER_FILES += deprecated/dialogSlider.ui       # Outdated and replaced in 02/2018, because the dialog became unuseable with HiDPI/4k Displays
OTHER_FILES += deprecated/QGMDialogSlider.h     # Outdated and replaced in 02/2018, because the dialog became unuseable with HiDPI/4k Displays
OTHER_FILES += deprecated/QGMDialogSlider.cpp   # Outdated and replaced in 02/2018, because the dialog became unuseable with HiDPI/4k Displays

DISTFILES += \
    gigamesh_warp_vertices.py \
	gigamesh_warp_vertices.py
