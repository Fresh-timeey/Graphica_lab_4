#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QLabel>



#include <QVector>

struct InsertedImage {
    QImage image;
    QPoint position;
};

struct InsertedText {
    QString text;
    QPoint position;
    QColor color;
    int brushSize;
};



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSave_As_triggered();
    void on_actionOpen_triggered();
    void on_actionColor_triggered();
    void on_actionBrush_Size_triggered();
    void on_actionFill_triggered();
    void on_actionText_triggered();
    void on_actionBlack_White_triggered();
    void on_actionInsert_Image_triggered();
    void on_actionHand_Mode_triggered();

    void on_actionrestart_triggered();

    void on_actionBlack_White_2_clicked();

    void on_actionBrush_Size_2_clicked();

    void on_actionColor_2_clicked();

    void on_actionFill_2_clicked();

    void on_actionHand_Mode_2_clicked();

    void on_actionInsert_Image_2_clicked();

    void on_actionText_2_clicked();

    void on_actionrestart_2_clicked();

    void on_actionSave_As_2_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void floodFill(QPixmap& canvas, const QPoint& startPoint, const QColor& fillColor);
private:
    Ui::MainWindow *ui;
    QPixmap canvas;
    QColor currentColor;
    int brushSize;
    bool drawing;
    bool handMode;
    QPoint lastPoint;
    QPoint dragStartPosition;
    QVector<InsertedImage> insertedImages;
    QVector<InsertedText> insertedTexts;



    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);
    void updateCanvasSize(const QSize &newSize);
    void toggleHandMode();
};

#endif // MAINWINDOW_H
