#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

static void do_drawing(cairo_t *, GtkWidget *, double user_data[]);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    double user_data[])
{
  do_drawing(cr, widget, user_data);  

  return FALSE; 
}

static void do_drawing(cairo_t *cr, GtkWidget *grid, double user_data[])
{
  
  //int width = 300; 
  //int height = 100;
  int i;
  double r = 50;
  double angle1 = 0;

  printf("in user_data %f: \n",user_data[0]);
  //cairo_translate(cr, width/4, height+10);

  for(i = 0; i < 7;i++){

    GtkWidget *darea;

    darea = gtk_drawing_area_new();

    gtk_grid_attach((GtkGrid *)grid, darea, i, 1, 1, 1);

    //gtk_widget_set_size_request(darea, width, height);

    cairo_set_line_width(cr, 0);
    cairo_set_source_rgb(cr, 0.12, 0.11, 0.77);
    cairo_move_to(cr,r,0);

    double angle2 = (2 * M_PI) * user_data[i];

    cairo_arc(cr, 0, 0, r, angle1, angle2);
    cairo_line_to (cr, 0, 0);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);


    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb(cr, 0.61, 0.77, 0.11);
    cairo_arc(cr, 0, 0, r, angle2, (2 * M_PI));
    cairo_stroke(cr);
    //cairo_translate(cr, width/2, 0);

  }     
}

int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *grid;


  int i;

  double P[7] = {0.3, 0.4, 0.15, 0.15, 0, 0, 0};

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  grid = gtk_grid_new();

  gtk_grid_set_column_homogeneous((GtkGrid *)grid,TRUE);
  gtk_grid_set_column_spacing((GtkGrid *)grid,30);

  gtk_container_add(GTK_CONTAINER(window), grid);

  for(i = 0; i < 7; i++)
  {
    GtkWidget *label;
    label = gtk_label_new("HELO");
    gtk_grid_attach((GtkGrid *)grid, label, i, 0, 1, 1);
    //gtk_widget_set_size_request(label, 150, 100);
  }

  g_signal_connect(G_OBJECT(grid), "draw",
      G_CALLBACK(on_draw_event), P);

  g_signal_connect(G_OBJECT(window), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  //gtk_window_set_default_size(GTK_WINDOW(window), 800, 500); 
  gtk_window_set_title(GTK_WINDOW(window), "Fill & stroke");

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
