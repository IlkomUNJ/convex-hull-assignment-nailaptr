#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <QVector>
#include <QPen>

class DrawingCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingCanvas(QWidget *parent = nullptr);
    void clearCanvas();
    void runSlowConvexHull();
    void runFastConvexHull();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QPoint> points;
    QVector<QPoint> hull;
    bool showHull;

    int slowIterations;
    int fastIterations;

    // Algorithms
    QVector<QPoint> slowConvexHull(QVector<QPoint> pts, int &iterations);
    QVector<QPoint> fastConvexHull(QVector<QPoint> pts, int &iterations);
    int orientation(const QPoint &p, const QPoint &q, const QPoint &r);

signals:
    void iterationUpdated(int slowIter, int fastIter);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRunSlow();
    void onRunFast();
    void onClear();
    void updateIterationLabels(int slowIter, int fastIter);

private:
    DrawingCanvas *canvas;
    QLabel *fastLabel;
    QLabel *slowLabel;
    QPushButton *fastButton;
    QPushButton *slowButton;
    QPushButton *clearButton;
};

#endif // MAINWINDOW_H
