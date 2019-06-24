#include "QGMDialogSliderHD.h"
#include "ui_QGMDialogSliderHD.h"

// generic C++ includes:
#include <iostream>
#include <cmath>

// Qt includes:
#include "QGMMacros.h"

using namespace std;

#define QGMDIALOGSLIDERHDDEFAULTS \
	mIndex( -1 ),           \
	mInitialValue( 0.5 ),   \
	mMinVal( 0.0 ),         \
	mMaxVal( 1.0 ),         \
	mFactor( 1.0 ),         \
	mLogarithmic( false )

//! Constructor.
QGMDialogSliderHD::QGMDialogSliderHD( QWidget *parent ) :
        QDialog( parent ), ui( new Ui::QGMDialogSliderHD ), QGMDIALOGSLIDERHDDEFAULTS
{
	ui->setupUi( this );

	// Set logo
	setWindowIcon( QIcon( _GIGAMESH_LOGO_ ) );

	// Setup validators and min/max
	mValidRel.setBottom( 0.0 );
	mValidRel.setTop( 1.0 );
	ui->lineValueRel->setValidator( &mValidRel );
	ui->lineValueAbs->setValidator( &mValidAbs );
	setMin( mMinVal );
	setMax( mMaxVal );

	// Setup signals
	QObject::connect( ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int))    );
	QObject::connect( ui->checkBoxPreview,  SIGNAL(toggled(bool)),     this, SLOT(previewChecked(bool)) );

	QObject::connect( ui->lineValueRel,     SIGNAL(textEdited(QString)), this, SLOT(valueChangedRel(QString)) );
	QObject::connect( ui->lineValueAbs,     SIGNAL(textEdited(QString)), this, SLOT(valueChangedAbs(QString)) );
}

//! Destructor.
QGMDialogSliderHD::~QGMDialogSliderHD() {
	delete( ui );
}

// --- Methods -------------------------------------------------------------------------

//! Set an index to be sent with a signal.
//! Returns the old value
int QGMDialogSliderHD::setIdx( int setIdx ) {
	int tmpVal = mMinVal;
	mIndex = setIdx;
	return( tmpVal );
}

//! Sets the minimum value of the slider.
//! Returns the old value.
double QGMDialogSliderHD::setMin( double setMinVal ) {
	setMinVal *= mFactor;
	double tmpVal = mMinVal;
	if( ui->horizontalSlider->invertedAppearance() ) {
		mMaxVal = setMinVal;
	} else {
		mMinVal = setMinVal;
	}
	mValidAbs.setBottom( setMinVal );
	ui->lineValueMin->setText( QString( "%1" ).arg( setMinVal ) );

	// Compute and show the stepping
	double steppingSlider = ( ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum() ) / ui->horizontalSlider->singleStep();
	double steppingAbsolut = ( mMaxVal - mMinVal ) / steppingSlider;
	ui->lineValueStep->setText( QString( "%1" ).arg( steppingAbsolut ) );

	return( tmpVal );
}

//! Sets the maximum value of the slider.
//! Returns the old value.
double QGMDialogSliderHD::setMax( double setMaxVal ) {
	setMaxVal *= mFactor;
	double tmpVal = mMaxVal;
	if( ui->horizontalSlider->invertedAppearance() ) {
		mMinVal = setMaxVal;
	} else {
		mMaxVal = setMaxVal;
	}
	mValidAbs.setTop( setMaxVal );
	ui->lineValueMax->setText( QString( "%1" ).arg( setMaxVal ) );

	// Compute and show the stepping
	double steppingSlider = ( ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum() ) / ui->horizontalSlider->singleStep();
	double steppingAbsolut = ( mMaxVal - mMinVal ) / steppingSlider;
	ui->lineValueStep->setText( QString( "%1" ).arg( steppingAbsolut ) );

	return( tmpVal );
}

//! Sets the maximum number of steps.
//! Returns the old value.
int QGMDialogSliderHD::setSteps( int rMaxSteps ) {
	int tmpVal = ui->horizontalSlider->maximum();
	ui->horizontalSlider->setMaximum( rMaxSteps );
	return tmpVal;
}

//! Sets the inital value of the slider.
//! ATTENTION: Has to be called AFTER setMin and setMax
//! Returns the old value.
double QGMDialogSliderHD::setPos( double setPos ) {
	setPos *= mFactor;
	if( mLogarithmic ) {
		setPos = log( setPos );
	}
	double tmpVal = mInitialValue;
	mInitialValue = setPos;

	int sliderPos = ( ( ( setPos - mMinVal ) / ( mMaxVal - mMinVal ) ) * ( static_cast<double>(ui->horizontalSlider->maximum()) - static_cast<double>(ui->horizontalSlider->minimum()) ) ) + ui->horizontalSlider->minimum();
	ui->horizontalSlider->setSliderPosition( sliderPos );
	valueChanged( sliderPos );

	return tmpVal;
}

//! Sets a multiplier.
//! Attention: Has to be called BEFORE setPos, setMin and setMax
//! Returns the old value.
double QGMDialogSliderHD::setFactor( double rFactor ) {
	double tmpVal = mFactor;
	mFactor = rFactor;
	return tmpVal;
}

//! Inverts the ruler e.g. for selection of values "bigger than".
//! Returns the old state.
//! Remarks: will be set to false, when accept() is called.
//! Should be called before setting min and max.
bool QGMDialogSliderHD::setInverted( bool setTo ) {
	bool tmpSet = ui->horizontalSlider->invertedAppearance();
	ui->horizontalSlider->setInvertedAppearance( setTo );
	return tmpSet;
}

//! Return logaritihmic instead of linear values.
bool QGMDialogSliderHD::setLogarithmic( bool rSetTo ) {
	mLogarithmic = rSetTo;
	return true;
}

//! Disables the preview checkbox until accept or reject are called.
//! Usefull, when called for a function having no preview capability.
void QGMDialogSliderHD::suppressPreview() {
	ui->checkBoxPreview->setChecked( false );
	ui->checkBoxPreview->setDisabled( true );
}

// --- Value Access --------------------------------------------------------------------------------------------------------------------------------------------

//! Get the selected value.
//! @returns false in case of an error.
bool QGMDialogSliderHD::getValue( double* rValue ) {
	int sliderPos = ui->horizontalSlider->sliderPosition();
	double valRel = ( static_cast<double>(sliderPos) - static_cast<double>(ui->horizontalSlider->minimum()) ) / ( static_cast<double>(ui->horizontalSlider->maximum()) - static_cast<double>(ui->horizontalSlider->minimum()) );
	double valSel = ( mMaxVal - mMinVal ) * valRel + mMinVal;
	if( mLogarithmic ) {
		valSel = exp( valSel );
	}
	valSel /= mFactor;
	(*rValue) = valSel;
	return true;
}

// --- SLOTS (private) -----------------------------------------------------------------------------------------------------------------------------------------

//! Sends an initial preview signal - see: valuePreview
void QGMDialogSliderHD::previewChecked( bool state ) {
	if( !state ) {
		return;
	}
	int sliderPos = ui->horizontalSlider->sliderPosition();
	valueChanged( sliderPos );
}

//! Accepts a double value as string, which is assumed to be checked by a QDoubleValidator before.
void QGMDialogSliderHD::valueChangedRel( const QString& rValRel ) {
	// BROKEN thanks to some locale mixup: hasAcceptableInput
	//if( !lineValueRel->hasAcceptableInput() ) {
	//	cerr << "[QGMDialogSliderHD::" << __FUNCTION__ << "] ERROR: NOT hasAcceptableInput!" << endl;
	//	QSETCOLOR( lineValueRel, 255, 128, 128 );
	//	return;
	//}
	QSETCOLOR( ui->lineValueRel, 255, 255, 255 );
	QSETCOLOR( ui->lineValueAbs, 255, 255, 255 );
	bool   convertOk;
	double valRel = rValRel.toDouble( &convertOk );
	if( !convertOk ) {
		cerr << "[QGMDialogSliderHD::" << __FUNCTION__ << "] ERROR: convert false!" << endl;
		return;
	}
	int sliderPos = valRel * ( ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum() ) + ui->horizontalSlider->minimum();
	QObject::disconnect( ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int))    );
	QObject::disconnect( ui->lineValueAbs,     SIGNAL(textEdited(QString)), this, SLOT(valueChangedAbs(QString)) );
	ui->horizontalSlider->setSliderPosition( sliderPos );
	double valPre = ( mMaxVal - mMinVal ) * valRel + mMinVal;
	if( mLogarithmic ) {
		valPre = exp( valPre );
	}
	ui->lineValueAbs->setText( tr( "%1" ).arg( valPre ) );
	QObject::connect( ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int))    );
	QObject::connect( ui->lineValueAbs,     SIGNAL(textEdited(QString)), this, SLOT(valueChangedAbs(QString)) );
	if( !ui->checkBoxPreview->isChecked() ) {
		return;
	}
	// Take care about preview:
	if( !ui->checkBoxPreview->isChecked() ) {
		return;
	}
	emit valuePreviewInt( static_cast<int>(round( valPre )) );
	emit valuePreviewFloat( valPre );
	emit valuePreviewIdFloat( mIndex, valPre );
}

//! Accepts a double value as string, which is assumed to be checked by a QDoubleValidator before.
void QGMDialogSliderHD::valueChangedAbs( const QString& rValAbs ) {
	// BROKEN thanks to some locale mixup: hasAcceptableInput
	//if( !lineValueAbs->hasAcceptableInput() ) {
	//	cerr << "[QGMDialogSliderHD::" << __FUNCTION__ << "] ERROR: NOT hasAcceptableInput!" << endl;
	//	QSETCOLOR( lineValueAbs, 255, 128, 128 );
	//	return;
	//}
	QSETCOLOR( ui->lineValueRel, 255, 255, 255 );
	QSETCOLOR( ui->lineValueAbs, 255, 255, 255 );
	bool   convertOk;
	double valAbs = rValAbs.toDouble( &convertOk );
	if( !convertOk ) {
		cerr << "[QGMDialogSliderHD::" << __FUNCTION__ << "] ERROR: convert false!" << endl;
		return;
	}
	if( mLogarithmic ) {
		valAbs = log( valAbs );
	}
	double valRel = ( valAbs - mMinVal ) / ( mMaxVal - mMinVal );
	int sliderPos = valRel * ( ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum() ) + ui->horizontalSlider->minimum();
	QObject::disconnect( ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int))    );
	QObject::disconnect( ui->lineValueRel,     SIGNAL(textEdited(QString)), this, SLOT(valueChangedRel(QString)) );
	ui->horizontalSlider->setSliderPosition( sliderPos );
	ui->lineValueRel->setText( tr( "%1" ).arg( valRel ) );
	QObject::connect( ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int))    );
	QObject::connect( ui->lineValueRel,     SIGNAL(textEdited(QString)), this, SLOT(valueChangedRel(QString)) );
	// Take care about preview:
	if( !ui->checkBoxPreview->isChecked() ) {
		return;
	}
	double valPre = ( mMaxVal - mMinVal ) * valRel + mMinVal;
	if( mLogarithmic ) {
		valPre = exp( valPre );
	}
	emit valuePreviewInt( static_cast<int>(round( valPre )) );
	emit valuePreviewFloat( valPre );
	emit valuePreviewIdFloat( mIndex, valPre );
}

//! Checks if preview is activated.
//! If so signals with selected values of the slider are emitted.
void QGMDialogSliderHD::valueChanged( int sliderPos ) {
	double valRel = ( static_cast<double>(sliderPos) - static_cast<double>(ui->horizontalSlider->minimum()) ) / ( static_cast<double>(ui->horizontalSlider->maximum()) - static_cast<double>(ui->horizontalSlider->minimum()) );
	double valPre = ( mMaxVal - mMinVal ) * valRel + mMinVal;
	if( mLogarithmic ) {
		valPre = exp( valPre );
	}
	ui->lineValueAbs->setText( tr( "%1" ).arg( valPre ) );
	ui->lineValueRel->setText( tr( "%1" ).arg( valRel ) );
	QSETCOLOR( ui->lineValueRel, 255, 255, 255 );
	QSETCOLOR( ui->lineValueAbs, 255, 255, 255 );
	if( !ui->checkBoxPreview->isChecked() ) {
		return;
	}
	emit valuePreviewInt( static_cast<int>(round( valPre )) );
	emit valuePreviewFloat( valPre/mFactor );
	emit valuePreviewIdFloat( mIndex, valPre/mFactor );
	//cout << __PRETTY_FUNCTION__ << " valPre: " << valPre << endl;
}

// --- SLOTS (public) --------------------------------------------------------------------

//! Checks if all the input values are correct. If the values seem to be correct
//! a signal is emitted, which should be connected MeshQt.
void QGMDialogSliderHD::accept() {
	int sliderPos = ui->horizontalSlider->sliderPosition();
	double valRel = ( static_cast<double>(sliderPos) - static_cast<double>(ui->horizontalSlider->minimum()) ) / ( static_cast<double>(ui->horizontalSlider->maximum()) - static_cast<double>(ui->horizontalSlider->minimum()) );
	double valSel = ( mMaxVal - mMinVal ) * valRel + mMinVal;
	if( mLogarithmic ) {
		valSel = exp( valSel );
	}
	//cout << "[QGMDialogSliderHD::" << __FUNCTION__ << "] valSel: " << valSel << endl;

	hide();

	emit valueSelected( static_cast<int>(round( valSel )) );
	emit valueSelected( valSel );
	emit valueSelected( mIndex, valSel );

	// set to default:
	ui->checkBoxPreview->setEnabled( true );
	ui->horizontalSlider->setInvertedAppearance( false );
	// to prevent multiple and wrong connections:
	QObject::disconnect();

	QDialog::accept();
}

//! Closes the dialog and emits the initial value in case preview has changed things.
void QGMDialogSliderHD::reject() {
	if(mLogarithmic)  {
		mInitialValue = exp( mInitialValue );
	}

	emit valuePreviewInt( mIndex );
	emit valuePreviewFloat( mInitialValue  );
	emit valuePreviewIdFloat( mIndex, mInitialValue  );

	// set to default:
	ui->checkBoxPreview->setEnabled( true );
	ui->horizontalSlider->setInvertedAppearance( false );
	// to prevent multiple and wrong connections:
	QObject::disconnect();

	QDialog::reject();
}
