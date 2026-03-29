#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>

// Dialog for configuring export settings
class ExportDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget* parent = nullptr);
    
    // Get chosen output path
    QString getOutputPath() const;
    // Get chosen export format
    QString getFormat() const;
    // Get quality spin box value
    int getQuality() const;
    void setOutputPath(const QString& path);
    void setFormat(const QString& format);
    
private slots:
    void browseOutputPath();
    
private:
    QLineEdit* pathEdit_;
    QComboBox* formatCombo_;
    QSpinBox* qualitySpinBox_;
    QPushButton* browseButton_;
    QPushButton* okButton_;
    QPushButton* cancelButton_;
};

#endif // EXPORTDIALOG_H
