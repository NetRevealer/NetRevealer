#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

static void do_drawing(cairo_t *, GtkWidget *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{  
  do_drawing(cr, widget);  

  return FALSE;
}

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  GtkWidget *win = gtk_widget_get_toplevel(widget);
  
  int width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  cairo_translate(cr, width/2, height/2);
  
 cairo_set_line_width (cr, 10.0);
 cairo_arc (cr, xc, yc, radius, angle1, angle2);
 cairo_stroke (cr);

 /* draw helping lines */
 cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
 cairo_set_line_width (cr, 6.0); 

 cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
 cairo_fill (cr);

 cairo_arc (cr, xc, yc, radius, angle1, angle1);
 cairo_line_to (cr, xc, yc);
 cairo_arc (cr, xc, yc, radius, angle2, angle2);
 cairo_line_to (cr, xc, yc);
 cairo_stroke (cr);
 
  //cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
  //cairo_fill(cr);      
}


int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *darea;
  
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), darea);

  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(G_OBJECT(window), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200); 
  gtk_window_set_title(GTK_WINDOW(window), "Fill & stroke");

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}