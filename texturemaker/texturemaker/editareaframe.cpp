#include "editareaframe.h"

//-----------------------------------------------------

editAreaFrame::editAreaFrame(QWidget *parent) :
    QFrame(parent)
{
   this->setAcceptDrops(true);
   this->selected_id = -1;
   this->moving = false;
   this->setFocusPolicy(Qt::StrongFocus);  // to accept key events
}

//-----------------------------------------------------

void editAreaFrame::set_main_window(MainWindow *main_window)

{
  this->main_window = main_window;
}

//-----------------------------------------------------

void editAreaFrame::paintEvent(QPaintEvent *)
{
  unsigned int i,j;
  c_block *block;
  c_texture_graph *graph;
  QString filename;
  QPixmap pixmap;
  QPixmap pixmap_block;
  QPixmap pixmap_slot[4];                     // all four directions
  int output_position[2],input_position[2];   // position differences for drawing slots
  t_block_position *position;

  QPainter painter(this);
  painter.fillRect(0,0,this->width(),this->height(),QColor::fromRgb(255,255,255));

  graph = this->main_window->get_texture_graph();

  pixmap_block.load(":/resources/block.png");

  pixmap_slot[0].load(":/resources/slot up.png");
  pixmap_slot[1].load(":/resources/slot right.png");
  pixmap_slot[2].load(":/resources/slot down.png");
  pixmap_slot[3].load(":/resources/slot left.png");

  for (i = 0; i < graph->get_number_of_blocks(); i++)
    {
      block = graph->get_block(i);
      filename = QString::fromStdString(":/resources/button " + block->get_name() + ".png");
      pixmap.load(filename);

      position = this->main_window->get_block_position(block->get_id());

      if (position == NULL)
        continue;

      if (((int) block->get_id()) == this->selected_id)                        // draw selection
        {
          QPen pen;

          pen.setColor(Qt::black);
          pen.setStyle(Qt::DashLine);

          painter.setPen(pen);
          painter.drawRect(position->x - 11,position->y - 9,75,75);
        }

      painter.drawPixmap(position->x,position->y,pixmap);                      // draw the block
      painter.drawPixmap(position->x - 2,position->y - 1,pixmap_block);

      switch (position->direction)
        {
          case 0:  // up
            output_position[0] = 22;
            output_position[1] = -5;
            input_position[0] = 6;
            input_position[1] = 56;
            break;

          case 1:  // right
            output_position[0] = 56;
            output_position[1] = 25;
            input_position[0] = -9;
            input_position[1] = 7;
            break;

          case 2:  // down
            output_position[0] = 22;
            output_position[1] = 56;
            input_position[0] = 6;
            input_position[1] = -5;
            break;

          case 3:  // left
            output_position[0] = -9;
            output_position[1] = 25;
            input_position[0] = 56;
            input_position[1] = 7;
            break;

          default:
            output_position[0] = 0;
            output_position[1] = 0;
            input_position[0] = 10;
            input_position[1] = 56;
            break;
        }

      // draw the output:

      if (!block->is_terminal())
        painter.drawPixmap(position->x + output_position[0],position->y + output_position[1],pixmap_slot[position->direction]);

      // draw input slots:

      for (j = 0; j < block->get_max_inputs(); j++)
        if (position->direction == 0 || position->direction == 2) // up or down
          painter.drawPixmap(position->x + input_position[0] + 8 * j,position->y + input_position[1],pixmap_slot[(position->direction + 2) % 4]);
        else
          painter.drawPixmap(position->x + input_position[0],position->y + input_position[1] + 9 * j,pixmap_slot[(position->direction + 2) % 4]);
    }
}

//-----------------------------------------------------

void editAreaFrame::dropEvent(QDropEvent *event)
{
  string mime_string = event->mimeData()->text().toUtf8().constData();
  c_block *block;
  t_block_position position;

  block = c_block::get_block_instance(mime_string);

  if (block == NULL)
    return;

  this->main_window->get_texture_graph()->add_block(block);
  position.block_id = block->get_id();
  position.x = event->pos().x();
  position.y = event->pos().y();
  position.direction = 0;
  this->main_window->set_block_position(position);

  this->update();    // repaint

  event->acceptProposedAction();
}

//-----------------------------------------------------

void editAreaFrame::dragEnterEvent(QDragEnterEvent *event)

{
  if (event->mimeData()->hasFormat("text/plain"))
    event->acceptProposedAction();
}

//-----------------------------------------------------

void editAreaFrame::mousePressEvent(QMouseEvent *event)

{
  int x, y;

  x = event->pos().x();
  y = event->pos().y();

  this->selected_id = this->main_window->get_block_by_position(x,y);

  if (this->selected_id >= 0)
    this->moving = true;

  this->update();
}

//-----------------------------------------------------

void editAreaFrame::keyPressEvent(QKeyEvent *event)

{
  if (this->selected_id < 0)
    return;

  if (event->key() == Qt::Key_Delete)
    this->main_window->delete_block_by_id(this->selected_id);

  this->update();
  this->selected_id = -1;
}

//-----------------------------------------------------

void editAreaFrame::mouseReleaseEvent(QMouseEvent *event)

{
  this->moving = false;
}

//-----------------------------------------------------

void editAreaFrame::mouseMoveEvent(QMouseEvent *event)

{
  if (this->moving)
    {
      t_block_position *position;

      position = this->main_window->get_block_position(this->selected_id);

      if (position == NULL)
        return;

      position->x = event->pos().x() - 25;
      position->y = event->pos().y() - 25;

      this->update();
    }
}

//-----------------------------------------------------

int editAreaFrame::get_selected_id()

{
  return this->selected_id;
}

//-----------------------------------------------------
