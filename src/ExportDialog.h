#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>

class ExportDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget* parent = nullptr);
    
    QString getOutputPath() const;
    QString getFormat() const;
    int getQuality() const;
    void setOutputPath(const QString& path);
    
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
