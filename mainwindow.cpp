#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QCursor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = QPixmap(size());
    canvas.fill(Qt::white);
    currentColor = Qt::black;
    brushSize = 5;
    drawing = false;
    handMode = false;
    setAcceptDrops(true);
    // ui->toolBar_2->setFixedWidth(100); // Установка фиксированной ширины
  //  ui->toolBar_2->setMinimumWidth(20); // Установка минимальной ширины
//
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateCanvasSize(event->size());
}

void MainWindow::updateCanvasSize(const QSize &newSize)
{
    if (canvas.size() == newSize)
        return;
    QPixmap newCanvas(newSize);
    newCanvas.fill(Qt::white);
    QPainter painter(&newCanvas);
    painter.drawPixmap(QPoint(0, 0), canvas);
    canvas = newCanvas;
    update();
}

void MainWindow::on_actionHand_Mode_triggered()
{
    handMode = !handMode;
    if (handMode) {
        ui->actionHand_Mode->setText("Выключить режим руки");
        setCursor(Qt::OpenHandCursor); // Устанавливаем курсор "ладонь"
    } else {
        ui->actionHand_Mode->setText("Включить режим руки");
        setCursor(Qt::ArrowCursor); // Возвращаем обычный курсор
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (handMode) {
        if (event->button() == Qt::LeftButton) {
            setCursor(Qt::ClosedHandCursor); // Курсор "сжатая ладонь" при нажатии
            dragStartPosition = event->pos();
        }
    } else {
        if (event->button() == Qt::LeftButton) {
            lastPoint = event->pos();
            drawing = true;
        }
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (handMode) {
        if (event->buttons() & Qt::LeftButton) {
            QPoint offset = event->pos() - dragStartPosition;
            dragStartPosition = event->pos();

            // Перемещение изображений
            for (auto &image : insertedImages) {
                image.position += offset;
            }

            // Перемещение текста
            for (auto &text : insertedTexts) {
                text.position += offset;
            }

            update();
        }
    } else {
        if ((event->buttons() & Qt::LeftButton) && drawing) {
            drawLineTo(event->pos());
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (handMode) {
        if (event->button() == Qt::LeftButton) {
            setCursor(Qt::OpenHandCursor);
        }
    } else {
        if (event->button() == Qt::LeftButton && drawing) {
            drawLineTo(event->pos());
            drawing = false;
        }
    }
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawPixmap(dirtyRect, canvas, dirtyRect);

    for (const auto &image : insertedImages) {
        painter.drawImage(image.position, image.image);
    }

    for (const auto &text : insertedTexts) {
        painter.setPen(QPen(text.color, text.brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawText(text.position, text.text);
    }
}

void MainWindow::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&canvas);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    lastPoint = endPoint;
    update();
}

void MainWindow::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Qt::white);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Files (*.png);;JPEG Files (*.jpg)"));
    if (!fileName.isEmpty()) {
        canvas.save(fileName);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        QImage loadedImage;
        if (loadedImage.load(fileName)) {
            resizeImage(&loadedImage, canvas.size());
            canvas = QPixmap::fromImage(loadedImage);
            update();
        }
    }
}

void MainWindow::on_actionColor_triggered()
{
    QColor newColor = QColorDialog::getColor(currentColor);
    if (newColor.isValid())
        currentColor = newColor;
}

void MainWindow::on_actionBrush_Size_triggered()
{
    bool ok;
    int newSize = QInputDialog::getInt(this, tr("Brush Size"), tr("Enter new brush size:"), brushSize, 1, 50, 1, &ok);
    if (ok)
        brushSize = newSize;
}




void MainWindow::on_actionFill_triggered()
{
    on_actionColor_triggered();
    floodFill(canvas, lastPoint, currentColor);
    QWidget:: update();
}
void MainWindow::floodFill(QPixmap& canvas, const QPoint& startPoint, const QColor& fillColor)
{
    QRect canvasRect(QPoint(0, 0), canvas.size());
    if(canvasRect.contains(startPoint)) {
        QImage image = canvas.toImage();
        QRgb targetColor = image.pixel(startPoint);
        if(targetColor == fillColor.rgb()) // Если цвет уже совпадает с цветом заливки, не делаем ничего
            return;

        QList<QPoint> stack;
        stack.append(startPoint);

        while(!stack.isEmpty()) {
            QPoint point = stack.takeLast();
            int x = point.x();
            int y = point.y();

            if(image.pixel(x, y) == targetColor) {
                image.setPixel(x, y, fillColor.rgb());

                if(x + 1 < canvas.width() && image.pixel(x + 1, y) == targetColor)
                    stack.append(QPoint(x + 1, y));

                if(x > 0 && image.pixel(x - 1, y) == targetColor)
                    stack.append(QPoint(x - 1, y));

                if(y + 1 < canvas.height() && image.pixel(x, y + 1) == targetColor)
                    stack.append(QPoint(x, y + 1));

                if(y > 0 && image.pixel(x, y - 1) == targetColor)
                    stack.append(QPoint(x, y - 1));
            }
        }

        canvas = QPixmap::fromImage(image);
    }
}






void MainWindow::on_actionText_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Insert Text"), tr("Enter text:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        InsertedText insertedText = { text, lastPoint, currentColor, brushSize };
        insertedTexts.append(insertedText);
        update();
    }
}

void MainWindow::on_actionInsert_Image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Insert Image"), "", tr("Images (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        QImage image;
        if (image.load(fileName)) {
            InsertedImage insertedImage = { image, lastPoint };
            insertedImages.append(insertedImage);
            update();
        }
    }
}


void MainWindow::on_actionBlack_White_triggered()
{
    QImage img = canvas.toImage();
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            int gray = qGray(img.pixel(x, y));
            img.setPixel(x, y, QColor(gray, gray, gray).rgb());
        }
    }
    canvas = QPixmap::fromImage(img);
    update();
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString fileName = urlList.at(0).toLocalFile();
            QImage image;
            if (image.load(fileName)) {
                resizeImage(&image, canvas.size());
                canvas = QPixmap::fromImage(image);
                update();
            }
        }
    }
}

void MainWindow::on_actionrestart_triggered()
{
    // Очищаем холст, заливая его белым цветом
    canvas.fill(Qt::white);
    // Очищаем все вставленные изображения и текстовые элементы
    insertedImages.clear();
    insertedTexts.clear();

    // Обновляем отображение
    update();
}


void MainWindow::on_actionBlack_White_2_clicked()
{
    on_actionBlack_White_triggered();
}


void MainWindow::on_actionBrush_Size_2_clicked()
{
   on_actionBrush_Size_triggered();
}


void MainWindow::on_actionColor_2_clicked()
{
    on_actionColor_triggered();
}


void MainWindow::on_actionFill_2_clicked()
{
    on_actionFill_triggered();
}


void MainWindow::on_actionHand_Mode_2_clicked()
{
    on_actionHand_Mode_triggered();
}


void MainWindow::on_actionInsert_Image_2_clicked()
{
    on_actionInsert_Image_triggered();
}


void MainWindow::on_actionText_2_clicked()
{
    on_actionText_triggered();
}

#include <QMessageBox>

void MainWindow::on_actionrestart_2_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите начать с чистого листа?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        on_actionrestart_triggered();
    }
}

void MainWindow::on_actionSave_As_2_clicked()
{
    on_actionSave_As_triggered();
}

