#include "NormalSphereSelectionDialog.h"
#include "ui_NormalSphereSelectionDialog.h"

#include "meshQt.h"
#include <QSlider>
#include <list>
#include <QAbstractButton>
#include <iostream>

NormalSphereSelectionDialog::NormalSphereSelectionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NormalSphereSelectionDialog),
	mMesh(nullptr)
{
	ui->setupUi(this);

	ui->colorMap_comboBox->addItem(tr("grayscale"));
	ui->colorMap_comboBox->addItem(tr("hot"));
	ui->colorMap_comboBox->addItem(tr("cold"));
	ui->colorMap_comboBox->addItem(tr("hsv"));
	ui->colorMap_comboBox->addItem(tr("hsv_part"));
	ui->colorMap_comboBox->addItem(tr("brewer_rdgy"));
	ui->colorMap_comboBox->addItem(tr("brewer_spectral"));
	ui->colorMap_comboBox->addItem(tr("brewer_rdylgn"));
	ui->colorMap_comboBox->addItem(tr("hypso"));
	ui->colorMap_comboBox->addItem(tr("octave_jet"));
	ui->colorMap_comboBox->addItem(tr("morgenstemning"));
	ui->colorMap_comboBox->addItem(tr("hypso_hirise1"));
	ui->colorMap_comboBox->addItem(tr("hypso_hirise2"));
	ui->colorMap_comboBox->addItem(tr("parula"));
	ui->colorMap_comboBox->addItem(tr("brewer_ylorbr"));
	ui->colorMap_comboBox->addItem(tr("octave_copper"));
	ui->colorMap_comboBox->addItem(tr("rusttones"));
	ui->colorMap_comboBox->addItem(tr("siennatones"));
	ui->colorMap_comboBox->addItem(tr("hypse_arid"));

	QObject::connect(ui->colorMap_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {this->ui->openGLWidget->setColorMapIndex(index);});

	ui->colorMap_comboBox->setCurrentIndex(11);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &NormalSphereSelectionDialog::comboButtonBoxClicked);
}

NormalSphereSelectionDialog::~NormalSphereSelectionDialog()
{
	delete ui;
}

void NormalSphereSelectionDialog::setMeshNormals(MeshQt* mesh)
{
	mMesh = mesh;
	std::vector<Vertex*> vertices;
	mesh->getVertexList(&vertices);

	std::vector<float> normalData;
	normalData.reserve(vertices.size() * 3);

	for(const auto vertex : vertices)
	{
		normalData.push_back(vertex->getNormalX());
		normalData.push_back(vertex->getNormalY());
		normalData.push_back(vertex->getNormalZ());
	}

	ui->openGLWidget->setRenderNormals(normalData);
}

void NormalSphereSelectionDialog::selectMeshByNormals()
{
	if(mMesh == nullptr)
		return;

	std::vector<Vertex*> vertices;
	mMesh->getVertexList(&vertices);

	std::list<Vertex*> selectedVertices;

	for(const auto vertex : vertices)
	{

		if(ui->openGLWidget->isNormalSelected(vertex->getNormalX(), vertex->getNormalY(), vertex->getNormalZ()))
		{
			selectedVertices.push_back(vertex);
		}
	}

	if(selectedVertices.empty())
		return;

	std::vector<Vertex*> selVertexVec;
	selVertexVec.reserve(selectedVertices.size());
	std::copy(selectedVertices.begin(), selectedVertices.end(), std::back_inserter(selVertexVec));


	mMesh->addToSelection(&selVertexVec);
}

void NormalSphereSelectionDialog::comboButtonBoxClicked(QAbstractButton* button)
{
	if(button == ui->buttonBox->button(QDialogButtonBox::Apply))
		selectMeshByNormals();

	else if(button == ui->buttonBox->button(QDialogButtonBox::Reset))
		ui->openGLWidget->clearSelected();
}
