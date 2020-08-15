#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <pthread.h>
#include <python3.8/Python.h>
#include <signal.h>
#include "../livefeaturecollector/LiveFeatureCollector.c"

#include "ip_management.c"

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
enum AppUsage_Cols {
    COL_AU_INDEX_APP,
    COL_AU_APP,
    COL_AU_NPACK,
    COL_AU_TOTALLEN,
    COL_AU_F_NPACK,
    COL_AU_F_TOTALLEN,
    COL_AU_B_NPACK,
    COL_AU_B_TOTALLEN,
    COL_AU_COLOR,
    NUM_AU_COLS
};

enum{
    COL_INDEX_SCAN,
    COL_FLOWID,
    COL_APP,
    COL_NPACK,
    COL_TOTALLEN,
    COL_MAXLEN,
    COL_MINLEN,
    COL_MEANLEN,
    COL_MAXIAT,
    COL_MINIAT,
    COL_TOTALIAT,
    COL_MEANIAT,
    COL_DURATION,
    COL_ACKCOUNT,
    COL_PSHCOUNT,
    COL_RSTCOUNT,
    COL_SYNCOUNT,
    COL_FINCOUNT,
    COL_TOTALWIN,
    COL_MAXWIN,
    COL_MINWIN,
    COL_MEANWIN,
    COL_F_NPACK,
    COL_F_TOTALLEN,
    COL_F_MAXLEN,
    COL_F_MINLEN,
    COL_F_MEANLEN,
    COL_F_MAXIAT,
    COL_F_MINIAT,
    COL_F_TOTALIAT,
    COL_F_MEANIAT,
    COL_F_DURATION,
    COL_F_ACKCOUNT,
    COL_F_PSHCOUNT,
    COL_F_RSTCOUNT,
    COL_F_SYNCOUNT,
    COL_F_FINCOUNT,
    COL_F_TOTALWIN,
    COL_F_MAXWIN,
    COL_F_MINWIN,
    COL_F_MEANWIN,
    COL_B_NPACK,
    COL_B_TOTALLEN,
    COL_B_MAXLEN,
    COL_B_MINLEN,
    COL_B_MEANLEN,
    COL_B_MAXIAT,
    COL_B_MINIAT,
    COL_B_TOTALIAT,
    COL_B_MEANIAT,
    COL_B_DURATION,
    COL_B_ACKCOUNT,
    COL_B_PSHCOUNT,
    COL_B_SYNCOUNT,
    COL_B_FINCOUNT,
    COL_B_TOTALWIN,
    COL_B_MAXWIN,
    COL_B_MINWIN,
    COL_B_MEANWIN,
    COL_COLOR,
    NUM_COLS
};

GObject *gtkScrolledWindow;

GtkWidget *view_Capture;
GtkTreeModel *model_Capture;
GtkListStore  *store_Capture;
GtkTreeIter iter_Capture;

GObject *gtkScrolledWindow2;

GtkWidget *view_AppUsage;
GtkTreeModel *model_AppUsage;
GtkListStore  *store_AppUsage;
GtkTreeIter iter_AppUsage;


GtkAdjustment * adj;

GObject *gtkToolBarStart;
GObject *gtkToolBarStop;
GObject *gtkToolBarRestart;
GObject *gtkToolBarStop;
GObject *gtkToolBarBlock;
GObject *gtkToolBarUnblock;
GObject *gtkToolBarUnblockAll;
GObject *gtkToolBarSave;
// GObject *gtkToolBarChangeNet;
GObject *gtkToolBarClear;
GObject *gtkToolBarQuitMain;
GObject *gtkWindowMain;
GObject *gtkToolBarBackupMain;
GObject *gtkToolBarRestoreMain;
GObject *gtkToolBarDefaultMain;
GObject *gtkLabelInterface;
GObject *gtkImagePlot;
GObject *gtkToolBarPlot;

int col_index = 0;
int app_index = 0;
gboolean plotButtonClicked = 0;

// Youtube, Twitch, Instagram, Googlemeet, Skype, Anghami, Others.
char *COLORS[7] = {"#ff9191","#b0ffff","#ffff99","#c6c6c6","#3936ff","#b4ffa3","#ffffff"};
gchar *devInterface;







pthread_t ptid_scan;
pthread_t ptid_insretProtoIP;
// pthread_t ptid_drawPlot;

void gtkToolBarRestore_clicked(GtkWidget *widget, gpointer data, GtkWindow *window);
void gtkToolBarBackup_clicked(GtkWidget *widget, gpointer data, GtkWindow *window);
void gtkToolBarDefault_clicked(GtkWidget *widget, gpointer data);
void gtkToolBarPlot_clicked(GtkWidget *widget, gpointer data){
    plotButtonClicked = 1;
    if (scan_stoped){

        char data_to_plot[1024] = "";
        gchar *app_name;
        int pack;
        int totlen;
        int fpack;
        int ftotlen;
        int bpack;
        int btotlen;
        
        gboolean valid = gtk_tree_model_get_iter_first (store_AppUsage,
                                        &iter_AppUsage);
        
        while (valid){
            gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                    COL_AU_APP, &app_name,
                    COL_AU_NPACK, &pack,
                    COL_AU_TOTALLEN, &totlen,
                    COL_AU_F_NPACK, &fpack,
                    COL_AU_F_TOTALLEN, &ftotlen,
                    COL_AU_B_NPACK, &bpack,
                    COL_AU_B_TOTALLEN, &btotlen,
                    -1);      

            strcat(data_to_plot, app_name);
            strcat(data_to_plot, ":");
            char converted_packs[50]; 
            sprintf(converted_packs, "%d", pack);
            strcat(data_to_plot, converted_packs);
            strcat(data_to_plot, ":");
            char converted_traffic_size[50]; 
            sprintf(converted_traffic_size, "%d", totlen);
            strcat(data_to_plot, converted_traffic_size);
            strcat(data_to_plot, ",");
        
        
            valid = gtk_tree_model_iter_next (store_AppUsage,
                                &iter_AppUsage);
        
        }

        // g_print("Getting elements is done! => %d \n", strlen(data_to_plot));
        
        if (strlen(data_to_plot) != 0){
            // g_print("Getting elements is done! => %s \n", data_to_plot);
            pargs_plot = Py_BuildValue("(s)", data_to_plot);
            PyObject_CallObject(plot_app_usage, pargs_plot);
            gtk_image_set_from_file (gtkImagePlot, ".appusage.png");
            
            
        }


    }
    
    
        

}

void gtkStoreAppend(gchar *data){
    gdk_threads_init ();
    gdk_threads_enter ();

    // printf("Hello\n");
    
    char *FLOWID = strtok(data, "|");
    char *APP = strtok(NULL, "|");
    int NPACK = atoi(strtok(NULL, "|"));
    int TOTALLEN = atoi(strtok(NULL, "|"));
    int MAXLEN = atoi(strtok(NULL, "|"));
    int MINLEN = atoi(strtok(NULL, "|"));
    double MEANLEN = atof(strtok(NULL, "|"));
    double MAXIAT = atof(strtok(NULL, "|"));
    double MINIAT = atof(strtok(NULL, "|"));
    double TOTALIAT = atof(strtok(NULL, "|"));
    double MEANIAT = atof(strtok(NULL, "|"));
    double DURATION = atof(strtok(NULL, "|"));
    int ACKCOUNT = atoi(strtok(NULL, "|"));
    int PSHCOUNT = atoi(strtok(NULL, "|"));
    int RSTCOUNT = atoi(strtok(NULL, "|"));
    int SYNCOUNT = atoi(strtok(NULL, "|"));
    int FINCOUNT = atoi(strtok(NULL, "|"));
    int TOTALWIN = atoi(strtok(NULL, "|"));
    int MAXWIN = atoi(strtok(NULL, "|"));
    int MINWIN = atoi(strtok(NULL, "|"));
    double MEANWIN = atof(strtok(NULL, "|"));
    int F_NPACK = atoi(strtok(NULL, "|"));
    int F_TOTALLEN = atoi(strtok(NULL, "|"));
    int F_MAXLEN = atoi(strtok(NULL, "|"));
    int F_MINLEN = atoi(strtok(NULL, "|"));
    double F_MEANLEN = atof(strtok(NULL, "|"));
    double F_MAXIAT = atof(strtok(NULL, "|"));
    double F_MINIAT = atof(strtok(NULL, "|"));
    double F_TOTALIAT = atof(strtok(NULL, "|"));
    double F_MEANIAT = atof(strtok(NULL, "|"));
    double F_DURATION = atof(strtok(NULL, "|"));
    int F_ACKCOUNT = atoi(strtok(NULL, "|"));
    int F_PSHCOUNT = atoi(strtok(NULL, "|"));
    int F_RSTCOUNT = atoi(strtok(NULL, "|"));
    int F_SYNCOUNT = atoi(strtok(NULL, "|"));
    int F_FINCOUNT = atoi(strtok(NULL, "|"));
    int F_TOTALWIN = atoi(strtok(NULL, "|"));
    int F_MAXWIN = atoi(strtok(NULL, "|"));
    int F_MINWIN = atoi(strtok(NULL, "|"));
    double F_MEANWIN = atof(strtok(NULL, "|"));
    int B_NPACK = atoi(strtok(NULL, "|"));
    int B_TOTALLEN = atoi(strtok(NULL, "|"));
    int B_MAXLEN = atoi(strtok(NULL, "|"));
    int B_MINLEN = atoi(strtok(NULL, "|"));
    double B_MEANLEN = atof(strtok(NULL, "|"));
    double B_MAXIAT = atof(strtok(NULL, "|"));
    double B_MINIAT = atof(strtok(NULL, "|"));
    double B_TOTALIAT = atof(strtok(NULL, "|"));
    double B_MEANIAT = atof(strtok(NULL, "|"));
    double B_DURATION = atof(strtok(NULL, "|"));
    int B_ACKCOUNT = atoi(strtok(NULL, "|"));
    int B_PSHCOUNT = atoi(strtok(NULL, "|"));
    int B_SYNCOUNT = atoi(strtok(NULL, "|"));
    int B_FINCOUNT = atoi(strtok(NULL, "|"));
    int B_TOTALWIN = atoi(strtok(NULL, "|"));
    int B_MAXWIN = atoi(strtok(NULL, "|"));
    int B_MINWIN = atoi(strtok(NULL, "|"));
    double B_MEANWIN = atof(strtok(NULL, "|"));
    

    gtk_list_store_append (store_Capture , &iter_Capture);
    gtk_list_store_set (store_Capture, &iter_Capture,
            COL_INDEX_SCAN, col_index,
            COL_FLOWID, FLOWID,
            COL_APP, APP,
            COL_NPACK, NPACK,
            COL_TOTALLEN, TOTALLEN,
            COL_MAXLEN, MAXLEN,
            COL_MINLEN, MINLEN,
            COL_MEANLEN, MEANLEN,
            COL_MAXIAT, MAXIAT,
            COL_MINIAT, MINIAT,
            COL_TOTALIAT, TOTALIAT,
            COL_MEANIAT, MEANIAT,
            COL_DURATION, DURATION,
            COL_ACKCOUNT, ACKCOUNT,
            COL_PSHCOUNT, PSHCOUNT,
            COL_RSTCOUNT, RSTCOUNT,
            COL_SYNCOUNT, SYNCOUNT,
            COL_FINCOUNT, FINCOUNT,
            COL_TOTALWIN, TOTALWIN,
            COL_MAXWIN, MAXWIN,
            COL_MINWIN, MINWIN,
            COL_MEANWIN, MEANWIN,
            COL_F_NPACK, F_NPACK,
            COL_F_TOTALLEN, F_TOTALLEN,
            COL_F_MAXLEN, F_MAXLEN,
            COL_F_MINLEN, F_MINLEN,
            COL_F_MEANLEN, F_MEANLEN,
            COL_F_MAXIAT, F_MAXIAT,
            COL_F_MINIAT, F_MINIAT,
            COL_F_TOTALIAT, F_TOTALIAT,
            COL_F_MEANIAT, F_MEANIAT,
            COL_F_DURATION, F_DURATION,
            COL_F_ACKCOUNT, F_ACKCOUNT,
            COL_F_PSHCOUNT, F_PSHCOUNT,
            COL_F_RSTCOUNT, F_RSTCOUNT,
            COL_F_SYNCOUNT, F_SYNCOUNT,
            COL_F_FINCOUNT, F_FINCOUNT,
            COL_F_TOTALWIN, F_TOTALWIN,
            COL_F_MAXWIN, F_MAXWIN,
            COL_F_MINWIN, F_MINWIN,
            COL_F_MEANWIN, F_MEANWIN,
            COL_B_NPACK, B_NPACK,
            COL_B_TOTALLEN, B_TOTALLEN,
            COL_B_MAXLEN, B_MAXLEN,
            COL_B_MINLEN, B_MINLEN,
            COL_B_MEANLEN, B_MEANLEN,
            COL_B_MAXIAT, B_MAXIAT,
            COL_B_MINIAT, B_MINIAT,
            COL_B_TOTALIAT, B_TOTALIAT,
            COL_B_MEANIAT, B_MEANIAT,
            COL_B_DURATION, B_DURATION,
            COL_B_ACKCOUNT, B_ACKCOUNT,
            COL_B_PSHCOUNT, B_PSHCOUNT,
            COL_B_SYNCOUNT, B_SYNCOUNT,
            COL_B_FINCOUNT, B_FINCOUNT,
            COL_B_TOTALWIN, B_TOTALWIN,
            COL_B_MAXWIN, B_MAXWIN,
            COL_B_MINWIN, B_MINWIN,
            COL_B_MEANWIN, B_MEANWIN,
            -1);
    // Youtube, Twitch, Instagram, Googlemeet, Skype, Anghami, Others.
    if (strcmp(APP, "Youtube") == 0)
        gtk_list_store_set (store_Capture, &iter_Capture, COL_COLOR, COLORS[0],-1);
    else if (strcmp(APP, "Twitch") == 0)
        gtk_list_store_set (store_Capture, &iter_Capture, COL_COLOR, COLORS[1],-1);
    else if (strcmp(APP, "Instagram") == 0)
        gtk_list_store_set (store_Capture, &iter_Capture, COL_COLOR, COLORS[2],-1);
    else if (strcmp(APP, "Googlemeet") == 0)
        gtk_list_store_set (store_Capture, &iter_Capture, COL_COLOR, COLORS[3],-1);
    else if (strcmp(APP, "Skype") == 0)
        gtk_list_store_set (store_Capture, &iter_Capture, COL_COLOR, COLORS[4],-1);
    else if (strcmp(APP, "Anghami") == 0)
        gtk_list_store_set (store_Capture, &iter_Capture, COL_COLOR, COLORS[5],-1);

    insert(APP,FLOWID);
    
    adj = gtk_scrolled_window_get_vadjustment (gtkScrolledWindow);
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
    col_index++;
    
    //AppUsage
    gboolean valid;
    gboolean done;

    gchar *app_name;
    int pack;
    int totlen;
    int fpack;
    int ftotlen;
    int bpack;
    int btotlen;

    
    // memset(data_to_plot, 0, 1024);
    done = FALSE;
    char data_to_plot[1024] = "";
    
    valid = gtk_tree_model_get_iter_first (store_AppUsage,
                                       &iter_AppUsage);
    
    while (valid){

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                            COL_AU_APP, &app_name,
                            -1);
        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                COL_AU_NPACK, &pack,
                COL_AU_TOTALLEN, &totlen,
                COL_AU_F_NPACK, &fpack,
                COL_AU_F_TOTALLEN, &ftotlen,
                COL_AU_B_NPACK, &bpack,
                COL_AU_B_TOTALLEN, &btotlen,
                -1);      
    

        if(strcmp(app_name, APP) == 0){                                                                   

            gtk_list_store_set(store_AppUsage,&iter_AppUsage,
                                2,(NPACK + pack),
                                3,(TOTALLEN / 1024) + totlen,
                                4,(F_NPACK + fpack),
                                5,(F_TOTALLEN / 1024) + ftotlen,
                                6,(B_NPACK + bpack),
                                7,(B_TOTALLEN / 1024) + btotlen,
                                -1);
                            
            done = TRUE;
            // break;                  
        }
        strcat(data_to_plot, app_name);
        strcat(data_to_plot, ":");
        char converted_packs[50]; 
        sprintf(converted_packs, "%d", pack);
        strcat(data_to_plot, converted_packs);
        strcat(data_to_plot, ":");
        char converted_traffic_size[50]; 
        sprintf(converted_traffic_size, "%d", totlen);
        strcat(data_to_plot, converted_traffic_size);
        strcat(data_to_plot, ",");
    
    
        valid = gtk_tree_model_iter_next (store_AppUsage,
                            &iter_AppUsage);
    
    }
    
    // g_print("%s\n", data_to_plot);
    
    // call to draw_plot

    if(!(done)){
        gtk_list_store_append (store_AppUsage , &iter_AppUsage);
        gtk_list_store_set (store_AppUsage, &iter_AppUsage,
            COL_AU_INDEX_APP, app_index,
            COL_AU_APP, APP,
            COL_AU_NPACK, NPACK,
            COL_AU_TOTALLEN, TOTALLEN / 1024,
            COL_AU_F_NPACK, F_NPACK,
            COL_AU_F_TOTALLEN, F_TOTALLEN / 1024,
            COL_AU_B_NPACK, B_NPACK,
            COL_AU_B_TOTALLEN, B_TOTALLEN / 1024,
            -1);
            if (strcmp(APP, "Youtube") == 0)
                gtk_list_store_set (store_AppUsage, &iter_AppUsage, COL_AU_COLOR, COLORS[0],-1);
            else if (strcmp(APP, "Twitch") == 0)
                gtk_list_store_set (store_AppUsage, &iter_AppUsage, COL_AU_COLOR, COLORS[1],-1);
            else if (strcmp(APP, "Instagram") == 0)
                gtk_list_store_set (store_AppUsage, &iter_AppUsage, COL_AU_COLOR, COLORS[2],-1);
            else if (strcmp(APP, "Googlemeet") == 0)
                gtk_list_store_set (store_AppUsage, &iter_AppUsage, COL_AU_COLOR, COLORS[3],-1);
            else if (strcmp(APP, "Skype") == 0)
                gtk_list_store_set (store_AppUsage, &iter_AppUsage, COL_AU_COLOR, COLORS[4],-1);
            else if (strcmp(APP, "Anghami") == 0)
                gtk_list_store_set (store_AppUsage, &iter_AppUsage, COL_AU_COLOR, COLORS[5],-1);

        app_index++;
    }

    if (plotButtonClicked){
        pargs_plot = Py_BuildValue("(s)", data_to_plot);
        PyObject *result = PyObject_CallObject(plot_app_usage, pargs_plot);
        gtk_image_set_from_file (gtkImagePlot, ".appusage.png");
        plotButtonClicked = 0;
    }
    
    gdk_threads_leave ();
    
    // if (plotButtonClicked){
    //     pargs_plot = Py_BuildValue("(s)", data_to_plot);
    //     PyObject *result = PyObject_CallObject(plot_app_usage, pargs_plot);
    //     gtk_image_set_from_file (gtkImagePlot, ".appusage.png");
    //     plotButtonClicked = 0;
    // }
    
    
    
}

static GtkTreeModel * create_and_fill_model_scan (void){
    
    store_Capture = gtk_list_store_new (NUM_COLS,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_STRING);
    // gtk_list_store_append (store_Capture , &iter_Capture);
    // gtk_list_store_set (store_Capture, &iter_Capture,
    //         COL_FLOWID, "token",
    //         COL_APP, "djamel",
    //         COL_NPACK, "999",
    //         -1);

    return GTK_TREE_MODEL (store_Capture);
};

static GtkWidget * create_view_and_model_scan (void){
    
    GtkCellRenderer     *renderer;
    GtkWidget           *view;
    GtkTreeViewColumn *column;

    view = gtk_tree_view_new ();

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N°",  
                                                renderer,
                                                "text", COL_INDEX_SCAN, 
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "FlowID",  
                                                renderer,
                                                "text", COL_FLOWID,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Type",  
                                                renderer,
                                                "text", COL_APP,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N° Pkts",  
                                                renderer,
                                                "text", COL_NPACK,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Total len",  
                                                renderer,
                                                "text", COL_TOTALLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Max len",  
                                                renderer,
                                                "text", COL_MAXLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Min len",  
                                                renderer,
                                                "text", COL_MINLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean len",  
                                                renderer,
                                                "text", COL_MEANLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Max IAT",  
                                                renderer,
                                                "text", COL_MAXIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean IAT",  
                                                renderer,
                                                "text", COL_MINIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Total IAT",  
                                                renderer,
                                                "text", COL_TOTALIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean IAT",  
                                                renderer,
                                                "text", COL_MEANIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Duration",  
                                                renderer,
                                                "text", COL_DURATION,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Ack count",  
                                                renderer,
                                                "text", COL_ACKCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Psh count",  
                                                renderer,
                                                "text", COL_PSHCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Rst count",  
                                                renderer,
                                                "text", COL_RSTCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Syn count",  
                                                renderer,
                                                "text", COL_SYNCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Fin count",  
                                                renderer,
                                                "text", COL_FINCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Total win",  
                                                renderer,
                                                "text", COL_TOTALWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Max win",  
                                                renderer,
                                                "text", COL_MAXWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Min win",  
                                                renderer,
                                                "text", COL_MINWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean win",  
                                                renderer,
                                                "text", COL_MEANWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N° F Pkts",  
                                                renderer,
                                                "text", COL_F_NPACK,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Total len",  
                                                renderer,
                                                "text", COL_F_TOTALLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Max len",  
                                                renderer,
                                                "text", COL_F_MAXLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Min len",  
                                                renderer,
                                                "text", COL_F_MINLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean len",  
                                                renderer,
                                                "text", COL_F_MEANLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Max IAT",  
                                                renderer,
                                                "text", COL_F_MAXIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean IAT",  
                                                renderer,
                                                "text", COL_F_MINIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Total IAT",  
                                                renderer,
                                                "text", COL_F_TOTALIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean IAT",  
                                                renderer,
                                                "text", COL_F_MEANIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Duration",  
                                                renderer,
                                                "text", COL_F_DURATION,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Ack count",  
                                                renderer,
                                                "text", COL_F_ACKCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Psh count",  
                                                renderer,
                                                "text", COL_F_PSHCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Rest count",  
                                                renderer,
                                                "text", COL_F_RSTCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Syn Count",  
                                                renderer,
                                                "text", COL_F_SYNCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Fin count",  
                                                renderer,
                                                "text", COL_F_FINCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Total win",  
                                                renderer,
                                                "text", COL_F_TOTALWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Max win",  
                                                renderer,
                                                "text", COL_F_MAXWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Min win",  
                                                renderer,
                                                "text", COL_F_MINWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean win",  
                                                renderer,
                                                "text", COL_F_MEANWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N° B Pkts",  
                                                renderer,
                                                "text", COL_B_NPACK,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Total len",  
                                                renderer,
                                                "text", COL_B_TOTALLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Max len",  
                                                renderer,
                                                "text", COL_B_MAXLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Min len",  
                                                renderer,
                                                "text", COL_B_MINLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean len",  
                                                renderer,
                                                "text", COL_B_MEANLEN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Max IAT",  
                                                renderer,
                                                "text", COL_B_MAXIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean IAT",  
                                                renderer,
                                                "text", COL_B_MINIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Total IAT",  
                                                renderer,
                                                "text", COL_B_TOTALIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean IAT",  
                                                renderer,
                                                "text", COL_B_MEANIAT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Duration",  
                                                renderer,
                                                "text", COL_B_DURATION,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Ack count",  
                                                renderer,
                                                "text", COL_B_ACKCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Psh count",  
                                                renderer,
                                                "text", COL_B_PSHCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Syn count",  
                                                renderer,
                                                "text", COL_B_SYNCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Fin count",  
                                                renderer,
                                                "text", COL_B_FINCOUNT,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Total win",  
                                                renderer,
                                                "text", COL_B_TOTALWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Max win",  
                                                renderer,
                                                "text", COL_B_MAXWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B min win",  
                                                renderer,
                                                "text", COL_B_MINWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean win",  
                                                renderer,
                                                "text", COL_B_MEANWIN,
                                                "cell-background",
                                                COL_COLOR,
                                                NULL);
    

    model_Capture = create_and_fill_model_scan ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model_Capture);

    for (int i =0; i < 59; i++){
        column = gtk_tree_view_get_column(view, i);
        gtk_tree_view_column_set_sort_column_id(column, i);
    }

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    return view;
};

static GtkTreeModel * create_and_fill_model_app (void){
    
    store_AppUsage = gtk_list_store_new (NUM_AU_COLS,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_STRING);

    return GTK_TREE_MODEL (store_AppUsage);
};

static GtkWidget * create_view_and_model_app (void){
    
    GtkCellRenderer     *renderer;
    GtkWidget           *view;
    GtkTreeViewColumn *column;

    view = gtk_tree_view_new ();

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N°",  
                                                renderer,
                                                "text", COL_AU_INDEX_APP,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);
    
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Application",  
                                                renderer,
                                                "text", COL_AU_APP,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Packs",  
                                                renderer,
                                                "text", COL_AU_NPACK,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Traffic size (KB)",  
                                                renderer,
                                                "text", COL_AU_TOTALLEN,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F_packs",  
                                                renderer,
                                                "text", COL_AU_F_NPACK,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Upload KB",  
                                                renderer,
                                                "text", COL_AU_F_TOTALLEN,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B_packs",  
                                                renderer,
                                                "text", COL_AU_B_NPACK,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Download KB",  
                                                renderer,
                                                "text", COL_AU_B_TOTALLEN,
                                                "cell-background",
                                                COL_AU_COLOR,
                                                NULL);                                                                                                                                                                                

    model_AppUsage = create_and_fill_model_app();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model_AppUsage);

    for (int i =0; i < 8; i++){
        column = gtk_tree_view_get_column(view, i);
        gtk_tree_view_column_set_sort_column_id(column, i);
    }

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model_AppUsage);

    return view;
};

void gtkToolBarChangeNet_clicked(GtkWidget *widget, gpointer data ){
    // gtk_window_close(gtkWindowMain);
    // starter(argcm, argvm);
    
    

}

void gtkToolBarStart_clicked(GtkWidget *widget, gpointer data) {
    // gtk_list_store_append (store_Capture , &iter_Capture);
    // gtk_list_store_set (store_Capture, &iter_Capture,
    //         COL_FLOWID, "token",
    //         COL_APP, "djamel",
    //         COL_NPACK, "999",
    //         -1);
    // while (!jacket_stoped)
    // {
    //     /* code */
    // }
    
    // memset(data_to_plot, 0, 1024);
    scan_stoped = 0;
    gtk_widget_set_sensitive(widget, FALSE);
    // if ((ptid_scan = fork()) == 0){
    //     scan(devInterface);
    // }
    // g_print("before start %d\n", ptid_scan);
    // ptid_scan = 0;
    pthread_create(&ptid_scan, NULL, &scan, devInterface);
    // g_print("after start %d\n", ptid_scan);
    // sleep(2);
    // scan(devInterface);
    
    gtk_widget_set_sensitive(gtkToolBarStop, TRUE);
    gtk_widget_set_sensitive(gtkToolBarRestart, TRUE);
    // g_print("start clicked \n");

    
    
    
    
}
void gtkToolBarStop_clicked(GtkWidget *widget, gpointer data) {
    // while (jacket_stoped)
    // {
    //     /* code */
    // }
    scan_stoped = 1;
    // pthread_kill(ptid_scan, 3);
    // pthread_join(ptid_scan, NULL);
    // kill(ptid_scan, SIGKILL);
    
    // g_print("before cancel %d\n", ptid_scan);
    while(TRUE){        
        if (liveCapture != NULL){
            pcap_breakloop(liveCapture);
            break;
        }
    }
    

    // pthread_cancel(ptid_scan);
    // g_print("after cancel %d\n", ptid_scan);
    
    gtk_widget_set_sensitive(widget, FALSE);
    gtk_widget_set_sensitive(gtkToolBarStart, TRUE);
    gtk_widget_set_sensitive(gtkToolBarRestart, FALSE);
    // g_print("stop clicked \n");

    // check_ipsets();
}

void gtkToolBarRestart_clicked(GtkWidget *widget, gpointer data) {
    // gtk_widget_set_sensitive(gtkToolBarRestart, FALSE);
    
    gtk_list_store_clear (store_Capture);
    gtk_list_store_clear (store_AppUsage);
    gtkToolBarStop_clicked(gtkToolBarStop, data);
    col_index = 0;
    app_index = 0;
    AppsToBlock[7] = NULL;
    init_AppIps();
    gtkToolBarStart_clicked(gtkToolBarStart, data);
    

}

void gtkButtonBlockApp_clicked(GtkWidget *widget, gpointer data) {
    if (currAppToBlock == NULL)
        return;

    
    for (int i = 0; i < 7; i++){
        if (AppsToBlock[i] == NULL){
            AppsToBlock[i] = currAppToBlock;
            break;
        }
    }
    block_App_init(currAppToBlock);
    gtk_window_close(widget);
    gtk_widget_set_sensitive(gtkWindowMain, TRUE);
}

void gtkButtonUnblockApp_clicked(GtkWidget *widget, gpointer data) {
    if (currAppToUnblock == NULL)
        return;
    
    
    for (int i = 0 ; i < 7; i++){
        if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], currAppToUnblock) ==0 ){
            int index = mapper(AppsToBlock[i]);
            AppsToBlock[i] = NULL;
            currAppToUnblock = NULL;
            IpProtoSet *set = APP_IPs[index];
            for (int j = 0 ; j < set->size; j++){
                char unblockAppCMD[512] = "iptables -D INPUT -s ";
                char * copy = malloc(strlen(set->ipset[j]) + 1); 
                strcpy(copy, set->ipset[j]);
                strtok(copy, "-");
                strcat(unblockAppCMD, strtok(NULL, "-"));
                strcat(unblockAppCMD, "/8 -j DROP");
                system(unblockAppCMD);
                g_print("unblocked ===> [%s]\n", unblockAppCMD);
            }
            break;
        }
    }
    gtk_window_close(widget);
    gtk_widget_set_sensitive(gtkWindowMain, TRUE);
}


void gtkWarningErrorQuitMain_clicked(GtkWidget *widget, gpointer data){
    gtk_window_close(widget);
    gtk_widget_set_sensitive(gtkWindowMain, TRUE);
    
}
void gtkErrorOkMain_clicked(GtkWidget *widget, gpointer data, GtkWidget *window) {
    gtk_window_close (window);
    gtk_widget_set_sensitive(gtkWindowMain, TRUE);
}

comboBoxBlock_on_changed (GtkComboBox *widget, gpointer   user_data){
    currAppToBlock = gtk_combo_box_text_get_active_text (widget);
}

comboBoxUnblock_on_changed (GtkComboBox *widget, gpointer   user_data){
    currAppToUnblock = gtk_combo_box_text_get_active_text (widget);
}

void gtkToolBarUnblockAll_clicked(GtkWidget *widget, gpointer data) {
    
    for (int i = 0 ; i < 7; i++){
        if (AppsToBlock[i] != NULL){
            g_print("\n%s\n\n", AppsToBlock[i]);
            int index = mapper(AppsToBlock[i]);
            AppsToBlock[i] = NULL;
            currAppToBlock = NULL;
            IpProtoSet *set = APP_IPs[index];
            for (int j = 0 ; j < set->size; j++){
                char unblockAppCMD[512] = "iptables -D INPUT -s ";
                char * copy = malloc(strlen(set->ipset[j]) + 1); 
                strcpy(copy, set->ipset[j]);
                strtok(copy, "-");
                strcat(unblockAppCMD, strtok(NULL, "-"));
                strcat(unblockAppCMD, "/8 -j DROP");
                system(unblockAppCMD);
                g_print("unblocked ===> [%s]\n", unblockAppCMD);
            }
        }
    }

}
void gtkToolBarUnblock_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *unblockAppBuilder;
    GtkWidget *gtkUnblockApp;
    GObject *gtkButtonUnblockApp;
    GObject *gtkButtonCancelUnblock;
    GtkComboBoxText *gtkComboBoxUnblock;
    GError *error = NULL;
    
    currAppToUnblock = NULL;
    
    gtk_widget_set_sensitive(gtkWindowMain, FALSE);
    unblockAppBuilder = gtk_builder_new ();
    if (gtk_builder_add_from_file (unblockAppBuilder, "unblock_app.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }
    gtkUnblockApp = gtk_builder_get_object (unblockAppBuilder, "gtkUnblockApp");
    gtkComboBoxUnblock = gtk_builder_get_object (unblockAppBuilder, "gtkComboBoxUnblock");
    gtkButtonUnblockApp = gtk_builder_get_object (unblockAppBuilder, "gtkButtonUnblockApp");
    gtkButtonCancelUnblock = gtk_builder_get_object (unblockAppBuilder, "gtkButtonCancelUnblock");
    
    g_signal_connect (gtkButtonUnblockApp, "clicked", gtkButtonUnblockApp_clicked, NULL);
    g_signal_connect (gtkButtonCancelUnblock, "clicked", gtkWarningErrorQuitMain_clicked, NULL);
    g_signal_connect (gtkUnblockApp, "destroy", gtkWarningErrorQuitMain_clicked, NULL);
    g_signal_connect (gtkComboBoxUnblock, "changed", G_CALLBACK (comboBoxUnblock_on_changed), NULL);
    for (int i = 0; i < 7; i++){
        if (AppsToBlock[i] != NULL){
            gtk_combo_box_text_append_text(gtkComboBoxUnblock, AppsToBlock[i]);
        }
            
    }
    
}
void gtkToolBarBlock_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *blockAppBuilder;
    GtkWidget *gtkBlockApp;
    GObject *gtkButtonBlockApp;
    GObject *gtkButtonCancelBlock;
    GtkComboBoxText *gtkComboBoxBlock;
    GError *error = NULL;
    
    currAppToBlock = NULL;
    gboolean found = FALSE;
    
    gtk_widget_set_sensitive(gtkWindowMain, FALSE);
    blockAppBuilder = gtk_builder_new ();
    if (gtk_builder_add_from_file (blockAppBuilder, "block_app.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }
    gtkBlockApp = gtk_builder_get_object (blockAppBuilder, "gtkBlockApp");
    gtkComboBoxBlock = gtk_builder_get_object (blockAppBuilder, "gtkComboBoxBlock");
    gtkButtonBlockApp = gtk_builder_get_object (blockAppBuilder, "gtkButtonBlockApp");
    gtkButtonCancelBlock = gtk_builder_get_object (blockAppBuilder, "gtkButtonCancelBlock");
    
    g_signal_connect (gtkButtonBlockApp, "clicked", gtkButtonBlockApp_clicked, NULL);
    g_signal_connect (gtkButtonCancelBlock, "clicked", gtkWarningErrorQuitMain_clicked, NULL);
    g_signal_connect (gtkBlockApp, "destroy", gtkWarningErrorQuitMain_clicked, NULL);
    g_signal_connect (gtkComboBoxBlock, "changed", G_CALLBACK (comboBoxBlock_on_changed), NULL);
    if (youtube.size > 0){
        for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Youtube") == 0){
                found = TRUE;
                break;
            }
            
        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Youtube");
    }

    found = FALSE;
    
    if (instagram.size > 0){
        for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Instagram") == 0){
                found = TRUE;
                break;
            }

        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Instagram");
    }

    found = FALSE;
    if (twitch.size > 0){
         for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Twitch") == 0){
                found = TRUE;
                break;
            }

        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Twitch");
    }

    found = FALSE;

    if (skype.size > 0){
        for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Skype") == 0){
                found = TRUE;
                break;
            }

        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Skype");
    }

    found = FALSE;

    if (googlemeet.size > 0){
        for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Googlemeet") == 0){
                found = TRUE;
                break;
            }

        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Googlemeet");
    }
    
    found = FALSE;
    if (anghami.size > 0){
        for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Anghami") == 0){
                found = TRUE;
                break;
            }

        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Anghami");
    }

    found = FALSE;
    if (others.size > 0){
        for (int i = 0; i < 7; i++){
            if (AppsToBlock[i] != NULL && strcmp(AppsToBlock[i], "Others") == 0){
                found = TRUE;
                break;
            }

        }
        if (!found)
            gtk_combo_box_text_append_text(gtkComboBoxBlock, "Others");
    }

}

void gtkToolBarSave_clicked(GtkWidget *widget, gpointer data, GtkWindow *window) {
    char *FLOWID;
    char *APP;
    int NPACK;
    int TOTALLEN;
    int MAXLEN;
    int MINLEN;
    double MEANLEN;
    double MAXIAT;
    double MINIAT;
    double TOTALIAT;
    double MEANIAT;
    double DURATION;
    int ACKCOUNT;
    int PSHCOUNT;
    int RSTCOUNT;
    int SYNCOUNT;
    int FINCOUNT;
    int TOTALWIN;
    int MAXWIN;
    int MINWIN;
    double MEANWIN;
    int F_NPACK;
    int F_TOTALLEN;
    int F_MAXLEN;
    int F_MINLEN;
    double F_MEANLEN;
    double F_MAXIAT;
    double F_MINIAT;
    double F_TOTALIAT;
    double F_MEANIAT;
    double F_DURATION;
    int F_ACKCOUNT;
    int F_PSHCOUNT;
    int F_RSTCOUNT;
    int F_SYNCOUNT;
    int F_FINCOUNT;
    int F_TOTALWIN;
    int F_MAXWIN;
    int F_MINWIN;
    double F_MEANWIN;
    int B_NPACK;
    int B_TOTALLEN;
    int B_MAXLEN;
    int B_MINLEN;
    double B_MEANLEN;
    double B_MAXIAT;
    double B_MINIAT;
    double B_TOTALIAT;
    double B_MEANIAT;
    double B_DURATION;
    int B_ACKCOUNT;
    int B_PSHCOUNT;
    int B_SYNCOUNT;
    int B_FINCOUNT;
    int B_TOTALWIN;
    int B_MAXWIN;
    int B_MINWIN;
    double B_MEANWIN;

    GtkBuilder *errorBuilder;
    GtkWidget *gtkErrorWidget;
    GObject *gtkErrorOk;
    GError *error = NULL;
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    FILE *fp ;
    gint res;
    gboolean valid = gtk_tree_model_get_iter_first (store_Capture,
                                       &iter_Capture);
    if (scan_stoped){
        
        if (!valid){
            gtk_widget_set_sensitive(gtkWindowMain, FALSE);
            errorBuilder = gtk_builder_new ();
            if (gtk_builder_add_from_file (errorBuilder, "error_emptyTable.ui", &error) == 0){
                g_printerr ("Error loading file: %s\n", error->message);
                g_clear_error (&error);
                return 1;
            }
            gtkErrorWidget = gtk_builder_get_object (errorBuilder, "gtkErrorEmptyTable");
            gtkErrorOk = gtk_builder_get_object (errorBuilder, "gtkErrorOk");
            g_signal_connect (gtkErrorOk, "clicked", gtkErrorOkMain_clicked, gtkErrorWidget);
            g_signal_connect (gtkErrorWidget, "destroy", gtkWarningErrorQuitMain_clicked, NULL);
            
        }
        else{
            dialog = gtk_file_chooser_dialog_new ("Save File",
                                        window,
                                        action,
                                        ("_Cancel"),
                                        GTK_RESPONSE_CANCEL,
                                        ("_Save"),
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
            chooser = GTK_FILE_CHOOSER (dialog);

            gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

            
            gtk_file_chooser_set_current_name (chooser,
                                                ("Untitled document.csv"));
            
            res = gtk_dialog_run (GTK_DIALOG (dialog));
            if (res == GTK_RESPONSE_ACCEPT){
                char *filename;
                filename = gtk_file_chooser_get_filename (chooser);
                fp = fopen(filename, "w");
                g_free (filename);
            }
            else {
                gtk_widget_destroy (dialog);
                return;
            }

            while (valid){

                gtk_tree_model_get (model_Capture, &iter_Capture, COL_FLOWID, &FLOWID, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_APP, &APP, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_NPACK, &NPACK, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_TOTALLEN, &TOTALLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MAXLEN, &MAXLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MINLEN, &MINLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MEANLEN, &MEANLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MAXIAT, &MAXIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MINIAT, &MINIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_TOTALIAT, &TOTALIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MEANIAT, &MEANIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_DURATION, &DURATION, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_ACKCOUNT, &ACKCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_PSHCOUNT, &PSHCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_RSTCOUNT, &RSTCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_SYNCOUNT, &SYNCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_FINCOUNT, &FINCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_TOTALWIN, &TOTALWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MAXWIN, &MAXWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MINWIN, &MINWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_MEANWIN, &MEANWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_NPACK, &F_NPACK, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_TOTALLEN, &F_TOTALLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MAXLEN, &F_MAXLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MINLEN, &F_MINLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MEANLEN, &F_MEANLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MAXIAT, &F_MAXIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MINIAT, &F_MINIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_TOTALIAT, &F_TOTALIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MEANIAT, &F_MEANIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_DURATION, &F_DURATION, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_ACKCOUNT, &F_ACKCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_PSHCOUNT, &F_PSHCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_RSTCOUNT, &F_RSTCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_SYNCOUNT, &F_SYNCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_FINCOUNT, &F_FINCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_TOTALWIN, &F_TOTALWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MAXWIN, &F_MAXWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MINWIN, &F_MINWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_F_MEANWIN, &F_MEANWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_NPACK, &B_NPACK, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_TOTALLEN, &B_TOTALLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MAXLEN, &B_MAXLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MINLEN, &B_MINLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MEANLEN, &B_MEANLEN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MAXIAT, &B_MAXIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MINIAT, &B_MINIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_TOTALIAT, &B_TOTALIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MEANIAT, &B_MEANIAT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_DURATION, &B_DURATION, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_ACKCOUNT, &B_ACKCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_PSHCOUNT, &B_PSHCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_SYNCOUNT, &B_SYNCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_FINCOUNT, &B_FINCOUNT, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_TOTALWIN, &B_TOTALWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MAXWIN, &B_MAXWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MINWIN, &B_MINWIN, -1);
                gtk_tree_model_get (model_Capture, &iter_Capture, COL_B_MEANWIN, &B_MEANWIN, -1);
                fprintf(fp,"%s,%s,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%f\n",
                    FLOWID,
                    APP,
                    NPACK,
                    TOTALLEN,
                    MAXLEN,
                    MINLEN,
                    MEANLEN,
                    MAXIAT,
                    MINIAT,
                    TOTALIAT,
                    MEANIAT,
                    DURATION,
                    ACKCOUNT,
                    PSHCOUNT,
                    RSTCOUNT,
                    SYNCOUNT,
                    FINCOUNT,
                    TOTALWIN,
                    MAXWIN,
                    MINWIN,
                    MEANWIN,
                    F_NPACK,
                    F_TOTALLEN,
                    F_MAXLEN,
                    F_MINLEN,
                    F_MEANLEN,
                    F_MAXIAT,
                    F_MINIAT,
                    F_TOTALIAT,
                    F_MEANIAT,
                    F_DURATION,
                    F_ACKCOUNT,
                    F_PSHCOUNT,
                    F_RSTCOUNT,
                    F_SYNCOUNT,
                    F_FINCOUNT,
                    F_TOTALWIN,
                    F_MAXWIN,
                    F_MINWIN,
                    F_MEANWIN,
                    B_NPACK,
                    B_TOTALLEN,
                    B_MAXLEN,
                    B_MINLEN,
                    B_MEANLEN,
                    B_MAXIAT,
                    B_MINIAT,
                    B_TOTALIAT,
                    B_MEANIAT,
                    B_DURATION,
                    B_ACKCOUNT,
                    B_PSHCOUNT,
                    B_SYNCOUNT,
                    B_FINCOUNT,
                    B_TOTALWIN,
                    B_MAXWIN,
                    B_MINWIN,
                    B_MEANWIN
                    );
                valid = gtk_tree_model_iter_next (store_Capture, &iter_Capture);

            
            }
        }
    }

    else {
        gtk_widget_set_sensitive(gtkWindowMain, FALSE);
        errorBuilder = gtk_builder_new ();
        if (gtk_builder_add_from_file (errorBuilder, "error_stopScan.ui", &error) == 0){
            g_printerr ("Error loading file: %s\n", error->message);
            g_clear_error (&error);
            return 1;
        }
        gtkErrorWidget = gtk_builder_get_object (errorBuilder, "gtkErrorStopScan");
        gtkErrorOk = gtk_builder_get_object (errorBuilder, "gtkErrorOk");
        g_signal_connect (gtkErrorOk, "clicked", gtkErrorOkMain_clicked, gtkErrorWidget);
        g_signal_connect (gtkErrorWidget, "destroy", gtkWarningErrorQuitMain_clicked, NULL);
    }

}


void gtkToolBarClear_clicked(GtkWidget *widget, gpointer data) {
    gtk_list_store_clear (store_Capture);
    gtk_list_store_clear (store_AppUsage);

    col_index = 0;
    app_index = 0;
}

int start (int   argc, char *argv[], gchar *dev){
    gchar *text;
    gint i;
    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolButton;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
    GError *error = NULL;
    // gdk_threads_init ();
    // gdk_threads_enter ();
    devInterface = dev;
    init_AppIps();

    gtk_init (&argc, &argv);
    
    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "main.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
        }

    /* Connect signal handlers to the constructed widgets. */
    gtkWindowMain = gtk_builder_get_object (builder, "main");
    gtkToolBarStart = gtk_builder_get_object(builder, "gtkToolBarStart");
    gtkToolBarRestart = gtk_builder_get_object(builder, "gtkToolBarRestart");
    gtkToolBarStop = gtk_builder_get_object(builder, "gtkToolBarStop");
    gtkToolBarBlock = gtk_builder_get_object(builder, "gtkToolBarBlock");
    gtkToolBarUnblock = gtk_builder_get_object(builder, "gtkToolBarUnblock");
    gtkToolBarUnblockAll = gtk_builder_get_object(builder, "gtkToolBarUnblockAll");
    gtkToolBarSave = gtk_builder_get_object(builder, "gtkToolBarSave");
    gtkToolBarClear = gtk_builder_get_object(builder, "gtkToolBarClear");
    gtkToolBarQuitMain = gtk_builder_get_object(builder, "gtkToolBarQuitMain");
    // gtkToolBarChangeNet = gtk_builder_get_object(builder, "gtkToolBarChangeNet");
    gtkToolBarBackupMain =  gtk_builder_get_object(builder, "gtkToolBarBackupMain");
    gtkToolBarRestoreMain =  gtk_builder_get_object(builder, "gtkToolBarRestoreMain");
    gtkToolBarDefaultMain =  gtk_builder_get_object(builder, "gtkToolBarDefaultMain");
    gtkImagePlot = gtk_builder_get_object(builder, "gtkImagePlot");
    gtkToolBarPlot = gtk_builder_get_object(builder, "gtkToolBarPlot");
    

    gtkScrolledWindow = gtk_builder_get_object(builder,"gtkScrolledWindow");
    gtkScrolledWindow2 = gtk_builder_get_object(builder,"gtkScrolledWindow2");

    gtkLabelInterface = gtk_builder_get_object(builder, "gtkLabelInterface");
    gtk_label_set_text(gtkLabelInterface, devInterface);
    gtk_widget_set_sensitive(gtkToolBarRestart, FALSE);

    
    
    view_Capture =  create_view_and_model_scan ();
    gtk_container_add (GTK_CONTAINER (gtkScrolledWindow), view_Capture);
    gtk_widget_set_sensitive(gtkToolBarStop, FALSE);


    //AppUsage 
    view_AppUsage =  create_view_and_model_app ();
    gtk_container_add (GTK_CONTAINER (gtkScrolledWindow2), view_AppUsage);
    gtk_widget_set_sensitive(gtkToolBarStop, FALSE);


    g_signal_connect (gtkToolBarStart, "clicked", gtkToolBarStart_clicked, NULL);
    g_signal_connect (gtkToolBarStop, "clicked", gtkToolBarStop_clicked, NULL);
    g_signal_connect (gtkToolBarRestart, "clicked", gtkToolBarRestart_clicked, NULL);
    g_signal_connect (gtkToolBarBlock, "clicked", gtkToolBarBlock_clicked, NULL);
    g_signal_connect (gtkToolBarUnblockAll, "clicked", gtkToolBarUnblockAll_clicked, NULL);
    g_signal_connect (gtkToolBarUnblock, "clicked", gtkToolBarUnblock_clicked, NULL);
    g_signal_connect (gtkToolBarSave, "clicked", gtkToolBarSave_clicked, gtkWindowMain);
    g_signal_connect (gtkToolBarClear, "clicked", gtkToolBarClear_clicked, NULL);
    g_signal_connect (gtkToolBarQuitMain, "clicked", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (gtkWindowMain, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (gtkToolBarBackupMain, "clicked", gtkToolBarBackup_clicked, NULL);
    g_signal_connect (gtkToolBarRestoreMain, "clicked", gtkToolBarRestore_clicked, NULL);
    // g_signal_connect (gtkToolBarChangeNet, "clicked", gtkToolBarChangeNet_clicked, NULL);
    g_signal_connect (gtkToolBarDefaultMain, "clicked", gtkToolBarDefault_clicked, NULL);
    g_signal_connect (gtkToolBarPlot, "clicked", gtkToolBarPlot_clicked, NULL);

    

    gtk_widget_show_all(gtkWindowMain);

    
    gtk_main ();
    // gdk_threads_leave ();

    return 0;
}
