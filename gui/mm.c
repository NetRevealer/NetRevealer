#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

// static GtkWidget * create_row (const gchar *text){
//   GtkWidget *row, *handle, *box, *label, *image;

//   row = gtk_list_box_row_new ();

//   box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
//   g_object_set (box, "margin-start", 10, "margin-end", 10, NULL);

//   handle = gtk_event_box_new ();
//   gtk_container_add (GTK_CONTAINER (box), handle);

//   label = gtk_label_new (text);
//   gtk_container_add_with_properties (GTK_CONTAINER (box), label, "expand", TRUE, NULL);
//   return row;
// }




// the struct of a data row that would be dispalyed 
typedef struct Rows{
    char app[10];
    unsigned short fpacks;
    unsigned short bpacks;
    char ip_set[6][16];
} Row;



//update the data when recieving new information
static void update_data(Row *data[], char ip[], char label[], int fpacks, int bpacks){

    int i = 0;
    int j = 0;
    for(i ; i < 7;i++)
    {
        if( strcmp(data[i]->app,label) == 0)
        {
            data[i]->fpacks += fpacks;
            data[i]->bpacks += bpacks;
            for(j; j < 6; j++)
            {

                if(data[i]->ip_set[j] == NULL)
                {   
                strcpy(data[i]->ip_set[j],ip);
                }

                else
                {
                    if(strcmp(data[i]->ip_set[j],ip) == 0)
                    {
                    break;
                    }
                }
            }

        }

        if( data[i]->app == NULL )
        {
            strcpy(data[i]->app, label);
            data[i]->fpacks += fpacks;
            data[i]->bpacks += bpacks;
            strcpy(data[i]->ip_set[j],ip);

        }
    }
};


enum{
  COL_APP,
  COL_NFPACK,
  COL_NBPACK,
  COL_NPACK,
  NUM_COLS
};

static GtkTreeModel * fill_model (Row *data[], GtkTreeIter iter, GtkListStore *store){
    
    int i;
    i = 0;

    while( data[i]->app != NULL)
    {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            COL_APP, data[i]->app,
                            COL_NFPACK, data[i]->fpacks,
                            COL_NBPACK, data[i]->bpacks,
                            COL_NPACK, (data[i]->fpacks + data[i]->fpacks),
                            -1);
    i++;                    
    }                    
    return GTK_TREE_MODEL (store);
};

static GtkWidget * create_view_from_model (GtkTreeModel *model){
    
    GtkCellRenderer     *renderer;
    GtkWidget           *view;




    view = gtk_tree_view_new ();

    /* --- Column #1 --- */

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Application",  
                                                renderer,
                                                "text", COL_APP,
                                                NULL);

    /* --- Column #2 --- */

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "# Packets",  
                                                renderer,
                                                "text", COL_NPACK,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Upload packets",  
                                                renderer,
                                                "text", COL_NFPACK,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Download packets",  
                                                renderer,
                                                "text", COL_NBPACK,
                                                NULL);                                                                                        

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
    
    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model);

    return view;
};


int main (int   argc, char *argv[]){
    gchar *text;
    GtkTreeIter    iter;
    GtkListStore  *store;

    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolButton;
    GObject *gtkGrid;
    GObject *gtkWindow;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
    GObject *gtkScrolledWindow;
    GError *error = NULL;
    GtkWidget *view;
    GtkTreeModel *model;

    int i;

    Row data[7];

    update_data(&data,"41.110.45.10","Youtube",20,20);

    printf("%s %d %d %d %s",data[0].app,data[0].fpacks,data[0].bpacks,data[0].ip_set[0]);
    /*
    gtk_init (&argc, &argv);

    //Construct a GtkBuilder instance and load our UI description
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "main.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
        }

    // Connect signal handlers to the constructed widgets
    gtkWindow = gtk_builder_get_object (builder, "main");
    gtkToolbar = gtk_builder_get_object(builder,"gtkToolBar");
    gtkGrid = gtk_builder_get_object(builder,"gtkGrid");

    gtkScrolledWindow = gtk_builder_get_object(builder,"gtkScrolledWindow");    
    gtk_widget_show(gtkWindow);

    store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT);
    view = gtk_tree_view_new ();
        
    model = fill_model (data,iter,store);
    view = create_view_from_model (model);
    gtk_container_add (GTK_CONTAINER (gtkScrolledWindow), view);
    gtk_widget_show(view);

    
    
    gtk_widget_show(view);
    g_signal_connect (gtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_main ();
    */

    return 0;
}
