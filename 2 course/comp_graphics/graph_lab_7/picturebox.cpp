#include <cmath>
#include <QDebug>
#include <QVector>
#include <QtTest/QTest>
#include <algorithm>
#include "picturebox.h"
#include "mainwindow.h"
using namespace std;

PictureBox::PictureBox(QWidget *parent) : QFrame(parent)
{
    resize(parent->width()-2, parent->height()-2); // размер как у родительского виджета, с небольшим отступом в 2 пикселя, чтобы не удалять оранжевую рамку фрейма
    m_Pixmap = QImage(size(),QImage::Format_RGB32); // инициализирую полотно, на котором буду рисовать
    m_Pixmap.fill(Qt::white); // заливаю белым цветом
    m_Grid = QPixmap(500,500); // создаю полотно для сетки, пусть будет 500x500
    m_Grid.fill(Qt::white); // ее тоже белым заливаю
    DrawGrid(); // формирую сетку и рисую ее на полотне, потом буду рисовать сетку как картинку, без повторного формирования сетки
    QPainter painter(&m_Pixmap);
    painter.drawPixmap(0,0,m_Grid); // рисую сетку в самом начале, при запуске программы
}

void PictureBox::DrawGrid()
{
    QPainter painter(&m_Grid);
    int w = m_Grid.width();
    int h = m_Grid.height();
    painter.setPen(Qt::green);
    for (int i = 0; i < h; i += 10)
    {
        painter.drawLine(0, i, w, i); // горизонтальная линия
        painter.drawLine(i, 0, i, h); // вертикальная
    }
}

void PictureBox::DrawDirectLine(QPoint q1, QPoint q2, QColor color, QPainter &painter, bool write_points)    // -- рисование отрезков через алгоритм Брезенхема -- //
{
    QVector<QPoint> intersections_points;
    int x1, y1,x2,y2, Px, Py, E;
    x1 = q1.x() / 10;
    y1 = q1.y() / 10;
    x2 = q2.x() / 10;
    y2 = q2.y() / 10;

    Px = abs(x2 - x1);
    Py = abs(y2 - y1);

    bool signx = x1 < x2;
    bool signy = y2 > y1;

    E = 0;
    painter.fillRect(x1*10+1,y1*10+1,9,9,color);
    intersections_points.push_back(QPoint(x1*10,y1*10));
    if (Px >= Py)
    {
        E = 2 * Py - Px;
        for (int i = 0; i < Px; ++i)
        {
            if (signx) ++x1;
            else --x1;
            if (E >= 0)
            {
                if (signy) ++y1;
                else --y1;
                E += 2 * (Py - Px);
            }
            else E += 2 * Py;
            painter.fillRect(x1*10+1,y1*10+1,9,9,color);
            intersections_points.push_back(QPoint(x1*10,y1*10));
        }
    }
    else
    {
        E = 2 * Px - Py;
        for (int i = 0; i < Py; ++i)
        {
            if (signy) ++y1;
            else --y1;
            if (E >= 0)
            {
                if (signx) ++x1;
                else --x1;
                E += 2 * (Px - Py);
            }
            else E += 2 * Px;
            painter.fillRect(x1*10+1,y1*10+1,9,9,color);
            intersections_points.push_back(QPoint(x1*10,y1*10));
        }
    }

    if (write_points) intersections.insert(intersections.end(),intersections_points.begin(),intersections_points.end());
}
void PictureBox::mousePressEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); // -- получаю координаты -- //
    p.rx() /= 10;   // -- округляю их вниз (например 347;284 -> 340;280)
    p.ry() /= 10;
    p *= 10;
    vertex.push_back(p);
    QPainter painter(&m_Pixmap);
    if (event->button() == Qt::LeftButton)
    {
        if (vertex.size() > 1)
        {
            DrawDirectLine(vertex[vertex.size()-2], vertex[vertex.size()-1],Qt::blue,painter,true);
        } else
        {
            painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::blue);
        }
    }
    if (event->button() == Qt::RightButton)
    {
       painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::blue);

    }
    if (event->button() == Qt::RightButton && event->modifiers() == Qt::ControlModifier)
    {
        painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::green);
        zatravka.push(p);
    }

    repaint();
}

void PictureBox::sort_intersections()
{
    // -- сортируем сверху вниз -- //
    std::sort(intersections.begin(),intersections.end(),
         [](const QPoint& l, const QPoint& r) {  // -- лямбда-функция - критерий -- //
            if (l.y() == r.y())
                return (l.x() <= r.x());
            return (l.y() < r.y());
         });
}

void PictureBox::fill_figure(QPainter &painter)
{
    int w = width();

    size_t N = intersections.size() - 1;
    for (size_t i = 0; i < N; ++i) // -- пройдемся по вектору с точками пересечений ребер и сканирующих строк -- //
    {
        QColor color = Qt::gray; // -- изначально цвет фона -- //
        const int x = intersections[i].x();
        const int y = intersections[i].y();
        DrawDirectLine(QPoint(0,y), QPoint(x-10,y),color,painter);  // -- заливаем слева от фигуры -- //
        while (intersections[i].y() == intersections[i+1].y() && i < N)
        {
            if (abs(intersections[i].x() - intersections[i+1].x()) > 10)    // -- если между двумя пересечениями есть пространство -- //
            {
                if (intersections[i-1].y() == intersections[i].y())
                {
                     if ((std::find(vertex.begin(),vertex.end(),intersections[i]) != vertex.end()))
                     {
                         if (color == Qt::gray) color = Qt::green;
                         else color = Qt::gray;
                     }
                }
                if (color == Qt::gray) color = Qt::green;
                else color = Qt::gray;
                int x = intersections[i].x() + 10;
                int y = intersections[i].y();
                int x2 = intersections[i+1].x() - 10;
                int y2 = intersections[i+1].y();
                DrawDirectLine(QPoint(x,y), QPoint(x2,y2),color,painter);
            }
            ++i;
        }
        DrawDirectLine(QPoint(intersections[i].x()+10,y), QPoint(w,y),Qt::gray,painter);
        MainWindow::wait(100);
        update();
    }
}

void PictureBox::fill() // -- заливаем фигуру -- //
{
    QPainter painter(&m_Pixmap);
    int h = height();
    int w = width();
    int min_y = h; // -- по умолчанию, если фигура не задана - зальем фоном фрейм (т.е заливаем до конца окна по 'y') -- //
    int max_y = h;
    if (!vertex.empty()) // -- если фигура нарисована, то заливаем до первой точки многоугольника -- //
    {
        min_y = intersections[0].y();
        max_y = intersections.back().y();
    }
    // -- заливаю фон до фигуры -- //
    for (int y = 0; y < min_y; y += 10)
    {
        DrawDirectLine(QPoint(0,y),QPoint(w,y),Qt::gray,painter);
        update();
        MainWindow::wait(100);

    }
    fill_figure(painter); // -- заливка фигуры -- //

    // -- заливаю фон после фигуры -- //
    for (int y = max_y+10; y < h; y += 10)
    {
        DrawDirectLine(QPoint(0,y),QPoint(w,y),Qt::gray,painter);
        update();
        MainWindow::wait(100);
    }
}

void PictureBox::risovanie()
{
    sort_intersections();   // -- сортирую вектор пересечений ребер и сканирующих строк -- //
    auto last = unique(intersections.begin()+1,intersections.end()-1); // -- удаляю дубликаты -- //
    intersections.erase(last,intersections.end()-1);
    fill();
}

void PictureBox::risovanie_zatravka()
{
    QPainter painter(&m_Pixmap);
    while (!zatravka.empty())
    {
        QPoint p = zatravka.top();
        zatravka.pop();

        if (m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::green)
        {
            painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::green);
            MainWindow::wait(3);
            repaint();
        }

        QPoint p_left(p.x()-10,p.y()), p_right(p.x()+10,p.y()), p_up(p.x(),p.y()+10), p_down(p.x(),p.y()-10);

        QColor p_left_color = m_Pixmap.pixelColor(p_left.x()+1,p_left.y()+1);
        QColor p_right_color = m_Pixmap.pixelColor(p_right.x()+1,p_right.y()+1);
        QColor p_up_color = m_Pixmap.pixelColor(p_up.x()+1,p_up.y()+1);
        QColor p_down_color = m_Pixmap.pixelColor(p_down.x()+1,p_down.y()+1);

        if ((p_right_color != (Qt::green)) && (p_right_color != (Qt::blue))) zatravka.push(p_right);
        if (p_up_color != (Qt::green) && p_up_color != (Qt::blue)) zatravka.push(p_up);
        if ((p_left_color != (Qt::green)) && (p_left_color != (Qt::blue))) zatravka.push(p_left);
        if (p_down_color != (Qt::green) && p_down_color != (Qt::blue)) zatravka.push(p_down);
    }
}
void PictureBox::zatravka_line_poisk_new_zatravka(QPoint p, int x_right)
{
    // проверим, что строка выше/ниже не является ни границей многоугольника, ни уже полностью
    // заполненной; если не так, то найдем затравку, начиная с левого края подинтервала
    // сканирующей строки
    while (p.x() <= x_right)
    {
        // -- ищем затравку на строке выше -- //
        bool flag = false;
        while (m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::blue &&
               m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::green &&
               p.x() < x_right)
        {
            if (!flag) flag = true;
            p.rx() += 10;
        }
        if (flag)
        {
            if (p.x() == x_right &&
                m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::blue &&
                m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::green)
            {
                zatravka.push(p);
            }
            else
            {
                zatravka.push(QPoint(p.x()-10,p.y()));
            }
            flag = 0;
        }
        // -- продолжаем проверку, если интервал прерван -- //
        int x_vhod = p.x();
        while (( m_Pixmap.pixelColor(p.x()+1,p.y()+1) == Qt::blue ||
               m_Pixmap.pixelColor(p.x()+1,p.y()+1) == Qt::green ) &&
               p.x() < x_right)
        {
            p.rx() += 10;
        }
        // -- удостоверимся, что координата пикселя увеличена -- //
        if (p.x() == x_vhod) p.rx() += 10;
    }
}

void PictureBox::risovanie_zatravka_line()
{
    QPainter painter(&m_Pixmap);
    while (!zatravka.empty())   // -- стек не пуст -- //
    {
        QPoint p = zatravka.top();  // -- извлекаем -- //
        zatravka.pop();
        painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::green); // -- заливаем пиксель -- //
        int temp_x = p.x(); // -- сохраняем x-координату затравочного пикселя -- //
        //----------------------------------------------//
        p.rx() += 10;   // -- начинаем заполнять интервал справа от затравки -- //
        // -- пока не дошли до границы -- //
        while (m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::blue)
        {
            painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::green); // -- заливаем пиксель -- //
            p.rx() += 10;
        }
        int x_right = p.x() - 10;   // -- сохраняем крайний пиксель справа -- //
        p.rx() = temp_x;    // -- восстанавливаем x-координату затравки -- //
        //----------------------------------------------//
        p.rx() -= 10; // -- начинаем заполнять интервал слева от затравки -- //
        // -- пока не дошли до границы -- //
        while (m_Pixmap.pixelColor(p.x()+1,p.y()+1) != Qt::blue)
        {
            painter.fillRect(p.x()+1,p.y()+1,9,9,Qt::green); // -- заливаем пиксель -- //
            p.rx() -= 10;
        }
        int x_left = p.x() + 10;   // -- сохраняем крайний пиксель слева -- //
        //----------------------------------------------//

        p.rx() = x_left;
        p.ry() -= 10;
        // -- смотрим строку выше и ищем новую затравку -- //
        zatravka_line_poisk_new_zatravka(p,x_right);
        p.ry() += 20;
        // -- смотрим строку ниже и ищем новую затравку -- //
        zatravka_line_poisk_new_zatravka(p,x_right);
        MainWindow::wait(30);
        repaint();
    }
}

void PictureBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(1,1,m_Pixmap);
}
