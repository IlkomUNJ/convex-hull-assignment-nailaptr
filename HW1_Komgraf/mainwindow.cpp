#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <algorithm>

// DrawingCanvas 
DrawingCanvas::DrawingCanvas(QWidget *parent)
    : QWidget(parent), showHull(false), slowIterations(0), fastIterations(0)
{
    setMinimumSize(800, 600);
    setStyleSheet("background-color: white; border: 1px solid black;");
}

void DrawingCanvas::clearCanvas()
{
    points.clear();
    hull.clear();
    showHull = false;
    slowIterations = 0;
    fastIterations = 0;
    update();
    emit iterationUpdated(slowIterations, fastIterations);
}

void DrawingCanvas::runSlowConvexHull()
{
    if (points.size() < 3) {
        QMessageBox::warning(this, "Warning", "Need at least 3 points!");
        return;
    }
    hull = slowConvexHull(points, slowIterations);
    showHull = true;
    update();
    emit iterationUpdated(slowIterations, fastIterations);
}

void DrawingCanvas::runFastConvexHull()
{
    if (points.size() < 3) {
        QMessageBox::warning(this, "Warning", "Need at least 3 points!");
        return;
    }
    hull = fastConvexHull(points, fastIterations);
    showHull = true;
    update();
    emit iterationUpdated(slowIterations, fastIterations);
}

void DrawingCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // draw points
    painter.setPen(QPen(Qt::black, 6));
    for (const QPoint &p : points) {
        painter.drawPoint(p);
    }

    // draw convex hull
    if (showHull && hull.size() >= 3) {
        painter.setPen(QPen(Qt::red, 2));
        for (int i = 0; i < hull.size(); i++) {
            QPoint p1 = hull[i];
            QPoint p2 = hull[(i + 1) % hull.size()];
            painter.drawLine(p1, p2);
        }
    }
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        points.append(event->pos());
        showHull = false;
        hull.clear();
        update();
    }
}

// Slow Convex Hull (O(n^3)) 
QVector<QPoint> DrawingCanvas::slowConvexHull(QVector<QPoint> pts, int &iterations)
{
    QVector<QPoint> hull;
    iterations = 0;
    int n = pts.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int pos = 0, neg = 0;
            for (int k = 0; k < n; k++) {
                if (k == i || k == j) continue;
                int ori = orientation(pts[i], pts[j], pts[k]);
                if (ori > 0) pos++;
                else if (ori < 0) neg++;
                iterations++;
            }
            if (pos == 0 || neg == 0) {
                hull.append(pts[i]);
                hull.append(pts[j]);
            }
        }
    }
    // remove duplicates & sort
    std::sort(hull.begin(), hull.end(), [](QPoint a, QPoint b) {
        return (a.x() < b.x()) || (a.x() == b.x() && a.y() < b.y());
    });
    hull.erase(std::unique(hull.begin(), hull.end()), hull.end());
    return hull;
}

// Fast Convex Hull (Jarvis March) 
QVector<QPoint> DrawingCanvas::fastConvexHull(QVector<QPoint> pts, int &iterations)
{
    iterations = 0;
    int n = pts.size();
    if (n < 3) return QVector<QPoint>();

    QVector<QPoint> hull;
    int l = 0;
    for (int i = 1; i < n; i++) {
        if (pts[i].x() < pts[l].x())
            l = i;
    }
    int p = l, q;
    do {
        hull.append(pts[p]);
        q = (p + 1) % n;
        for (int i = 0; i < n; i++) {
            if (orientation(pts[p], pts[i], pts[q]) == -1) {
                q = i;
            }
            iterations++;
        }
        p = q;
    } while (p != l);
    return hull;
}

// orientation helper
int DrawingCanvas::orientation(const QPoint &p, const QPoint &q, const QPoint &r)
{
    long val = (q.y() - p.y()) * (r.x() - q.x()) -
               (q.x() - p.x()) * (r.y() - q.y());
    if (val == 0) return 0;
    return (val > 0) ? 1 : -1; // 1 = clockwise, -1 = counterclockwise
}

// MainWindow 
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Convex Hull Comparison");
    resize(900, 700);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // iteration labels
    fastLabel = new QLabel("Fast Iterations: 0");
    slowLabel = new QLabel("Slow Iterations: 0");
    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->addWidget(fastLabel);
    labelLayout->addWidget(slowLabel);
    mainLayout->addLayout(labelLayout);

    // canvas
    canvas = new DrawingCanvas(this);
    mainLayout->addWidget(canvas);

    // buttons
    fastButton = new QPushButton("Run Fast Convex Hull");
    slowButton = new QPushButton("Run Slow Convex Hull");
    clearButton = new QPushButton("Clear");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(fastButton);
    buttonLayout->addWidget(slowButton);
    buttonLayout->addWidget(clearButton);

    mainLayout->addLayout(buttonLayout);

    setCentralWidget(central);

    // connect signals
    connect(fastButton, &QPushButton::clicked, this, &MainWindow::onRunFast);
    connect(slowButton, &QPushButton::clicked, this, &MainWindow::onRunSlow);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClear);
    connect(canvas, &DrawingCanvas::iterationUpdated, this, &MainWindow::updateIterationLabels);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onRunSlow()
{
    canvas->runSlowConvexHull();
}

void MainWindow::onRunFast()
{
    canvas->runFastConvexHull();
}

void MainWindow::onClear()
{
    canvas->clearCanvas();
}

void MainWindow::updateIterationLabels(int slowIter, int fastIter)
{
    fastLabel->setText(QString("Fast Iterations: %1").arg(fastIter));
    slowLabel->setText(QString("Slow Iterations: %1").arg(slowIter));
}
