#include "ExportDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDialogButtonBox>

ExportDialog::ExportDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Export Texture");
    setMinimumWidth(500);
    
    // Output path
    pathEdit_ = new QLineEdit;
    browseButton_ = new QPushButton("Browse...");
    connect(browseButton_, &QPushButton::clicked, this, &ExportDialog::browseOutputPath);
    
    QHBoxLayout* pathLayout = new QHBoxLayout;
    pathLayout->addWidget(pathEdit_);
    pathLayout->addWidget(browseButton_);
    
    // Format selection
    formatCombo_ = new QComboBox;
    formatCombo_->addItem("PNG", "png");
    formatCombo_->addItem("JPEG", "jpg");
    formatCombo_->addItem("BMP", "bmp");
    formatCombo_->addItem("TGA", "tga");
    formatCombo_->addItem("TIFF", "tiff");
    
    // Quality setting (for JPEG)
    qualitySpinBox_ = new QSpinBox;
    qualitySpinBox_->setRange(1, 100);
    qualitySpinBox_->setValue(90);
    qualitySpinBox_->setSuffix("%");
    
    // Form layout
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("Output Directory:", pathLayout);
    formLayout->addRow("Format:", formatCombo_);
    formLayout->addRow("Quality (JPEG):", qualitySpinBox_);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
}

void ExportDialog::browseOutputPath() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory",
                                                     pathEdit_->text());
    if (!dir.isEmpty()) {
        pathEdit_->setText(dir);
    }
}

QString ExportDialog::getOutputPath() const {
    return pathEdit_->text();
}

QString ExportDialog::getFormat() const {
    return formatCombo_->currentData().toString();
}

int ExportDialog::getQuality() const {
    return qualitySpinBox_->value();
}

void ExportDialog::setOutputPath(const QString& path) {
    pathEdit_->setText(path);
}

void ExportDialog::setFormat(const QString& format) {
    int index = formatCombo_->findData(format);
    if (index >= 0) {
        formatCombo_->setCurrentIndex(index);
    }
}
