/********************************************************************
 *                                       (c) Andrzej Lukaszewski 2009
 * Gimp Plugin  :
 *             Simple plugin template with dialog
 *
 * This file: gimp plugin interface
 ********************************************************************/
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

struct
{
	gdouble a1;
	gdouble a2;
} config = { 0.1, 0.1 };

// Function to set up and call CPU processing (implemented elsewhere)
void cpu_process(unsigned char *buffer,int width,int height,int channels,
				 double a1, double a2);


#define DIALOG
// ------------------------------------------------------------------
#ifdef DIALOG
gboolean dialog (GimpDrawable *drawable)
{
	GtkWidget *dialog;
	GtkWidget *main_vbox;
	GtkWidget *main_hbox;

	GtkWidget *a1_label;
	GtkWidget *a2_label;
	GtkWidget *a1_d;
	GtkWidget *a2_d;
	gboolean   run;
	GtkObject *a1_adj_d;
	GtkObject *a2_adj_d;

	gimp_ui_init("Coefficients", FALSE);
	dialog = gimp_dialog_new("Coefficients", "Coefficients",
							 NULL, (GtkDialogFlags) 0,
							 gimp_standard_help_func, "plug-in-distortion-dialog",
							 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							 GTK_STOCK_OK,     GTK_RESPONSE_OK,
							 NULL);
	main_vbox = gtk_vbox_new(FALSE, 6);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG (dialog)->vbox), main_vbox);
	gtk_widget_show(main_vbox);

	// a1 input:
	a1_label = gtk_label_new("a1:");
	gtk_widget_show(a1_label);
	gtk_box_pack_start(GTK_BOX (main_vbox), a1_label, FALSE, FALSE, 6);
	gtk_label_set_justify(GTK_LABEL (a1_label), GTK_JUSTIFY_RIGHT);

	a1_d = gimp_spin_button_new(&a1_adj_d, config.a1,
									-1.0, 1.0, 0.001, 0.001, 0, 30, 3);
	gtk_box_pack_start(GTK_BOX(main_vbox), a1_d, FALSE, FALSE, 0);
	gtk_widget_show(a1_d);
	g_signal_connect(a1_adj_d, "value_changed",
					 G_CALLBACK(gimp_double_adjustment_update),
					 &config.a1);
	// a2 input:
	a2_label = gtk_label_new("a2:");
	gtk_widget_show(a2_label);
	gtk_box_pack_start(GTK_BOX (main_vbox), a2_label, FALSE, FALSE, 6);
	gtk_label_set_justify(GTK_LABEL (a2_label), GTK_JUSTIFY_RIGHT);

	a2_d = gimp_spin_button_new(&a2_adj_d, config.a2,
									-1.0, 1.0, 0.001, 0.001, 0, 30, 3);
	gtk_box_pack_start(GTK_BOX(main_vbox), a2_d, FALSE, FALSE, 0);
	gtk_widget_show(a2_d);
	g_signal_connect(a2_adj_d, "value_changed",
					 G_CALLBACK(gimp_double_adjustment_update),
					 &config.a2);


	gtk_widget_show(dialog);

	run = (gimp_dialog_run(GIMP_DIALOG(dialog)) == GTK_RESPONSE_OK);

	gtk_widget_destroy(dialog);
	return run;
}
#endif
// ------------------------------------------------------------------



static void query(void)
{
	static GimpParamDef args[]= {
		{GIMP_PDB_INT32   ,"run-mode","Run mode"      },
		{GIMP_PDB_IMAGE   ,"image"   ,"Input image"   },
		{GIMP_PDB_DRAWABLE,"drawable","Input drawable"}
#ifdef DIALOG
		,{GIMP_PDB_INT32   ,"gpu"     ,"Run on GPU"    }   // dialog
		,{GIMP_PDB_INT32   ,"R"       ,"radius"    }   // dialog
#endif
	};

	gimp_install_procedure(
		"plug-in-distortion-dialog",
		"plug-in=cuda-test2",
		"plug-in-cuda-test3",
		"anl",
		"Copyright A.Łukaszewski",
		"2009",
		"Korekcja dystorsji radialnej",
		"RGB",
		GIMP_PLUGIN,
		G_N_ELEMENTS (args), 0,
		args, NULL
	);

	gimp_plugin_menu_register("plug-in-distortion-dialog","<Image>/Filters/Misc");
}


static void render(GimpDrawable *drawable)
{
	gint         channels;
	gint         x1,y1,x2,y2;
	GimpPixelRgn src,dst;
	guchar       *buffer;
	gint         width,height;

	gimp_drawable_mask_bounds(drawable->drawable_id,&x1,&y1,&x2,&y2);
	channels=gimp_drawable_bpp(drawable->drawable_id);

	width =x2-x1;
	height=y2-y1;

	gimp_pixel_rgn_init(&src,drawable,x1,y1,width,height,FALSE,FALSE);
	gimp_pixel_rgn_init(&dst,drawable,x1,y1,width,height,TRUE ,TRUE );

	buffer=g_new(guchar,channels*width*height);

	gimp_pixel_rgn_get_rect(&src,buffer,x1,y1,width,height);

	//if (config.gpu)
	//   gpu_process(buffer,width,height,channels);
	//else
	cpu_process(buffer,width,height,channels, config.a1, config.a2);

	gimp_pixel_rgn_set_rect(&dst,buffer,x1,y1,width,height);

	g_free(buffer);

	gimp_drawable_flush(drawable);
	gimp_drawable_merge_shadow(drawable->drawable_id,TRUE);
	gimp_drawable_update(drawable->drawable_id,x1,y1,width,height);
}


static void run(const gchar *name,gint nparams,const GimpParam *param,gint *nreturn_vals,GimpParam **return_vals)
{
	static GimpParam  values[1];
	GimpPDBStatusType status   = GIMP_PDB_SUCCESS;
	GimpRunMode       run_mode = (GimpRunMode)param[0].data.d_int32;
	GimpDrawable      *drawable;

	// Setting mandatory output values
	*nreturn_vals=1;
	*return_vals =values;

	values[0].type          = GIMP_PDB_STATUS;
	values[0].data.d_status = status;

	// Get specified drawable
	drawable=gimp_drawable_get(param[2].data.d_drawable);

#ifdef DIALOG
	if (run_mode == GIMP_RUN_INTERACTIVE) {
		gimp_get_data("plug-in-cuda-dialog", &config);
		dialog(drawable);
	}
#endif

	render(drawable);

	if (run_mode!=GIMP_RUN_NONINTERACTIVE) {
		//if (config.gpu)
		//   g_message("GPU Filtering 'anl CUDA test' done !\n");
		// else
		g_message("CPU Filtering 'anl CUDA test' done !\n");
	}

	gimp_displays_flush();
	gimp_drawable_detach(drawable);
}


GimpPlugInInfo PLUG_IN_INFO= {
	NULL,
	NULL,
	query,
	run
};

MAIN()
