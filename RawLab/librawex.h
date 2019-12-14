// LibRawEx.h : LibRaw extension class

typedef struct
{
	//  unsigned greybox[4];   /* -A  x1 y1 x2 y2 */
	//  unsigned cropbox[4];   /* -B x1 y1 x2 y2 */
	//  double aber[4];        /* -C */
	//  double gamm[6];        /* -g */
	//  float user_mul[4];     /* -r mul0 mul1 mul2 mul3 */
	//  unsigned shot_select;  /* -s */
	//  float bright;          /* -b */
	//  float threshold;       /*  -n */
	//  int half_size;         /* -h */
	//  int four_color_rgb;    /* -f */
	//  int highlight;         /* -H */
	//  int use_auto_wb;       /* -a */
	//  int use_camera_wb;     /* -w */
	//  int use_camera_matrix; /* +M/-M */
	//  int output_color;      /* -o */
	//  char *output_profile;  /* -o */
	//  char *camera_profile;  /* -p */
	//  char *bad_pixels;      /* -P */
	//  char *dark_frame;      /* -K */
	//  int output_bps;        /* -4 */
	//  int output_tiff;       /* -T */
	//  int user_flip;         /* -t */
	//  int user_qual;         /* -q */
	//  int user_black;        /* -k */
	//  int user_cblack[4];
	//  int user_sat; /* -S */

	//  int med_passes; /* -m */
	//  float auto_bright_thr;
	//  float adjust_maximum_thr;
	//  int no_auto_bright;  /* -W */
	//  int use_fuji_rotate; /* -j */
	//  int green_matching;
	  /* DCB parameters */
	//  int dcb_iterations;
	//  int dcb_enhance_fl;
	//  int fbdd_noiserd;
	  /* VCD parameters */
	int eeci_refine;	// Use EECI refine after VCD+AHD demosaic (user_qual = 8)
	int es_med_passes;	// Number of edge-sensitive median filter passes after VCD+AHD demosaic (user_qual = 8)
	/* AMaZE*/
	int ca_correc;
	float cared;
	float cablue;
	int cfaline;
	float linenoise;
	int cfa_clean;
	float lclean;
	float cclean;
	//  int cfa_green; // Reduces maze artifacts produced by bad balance of green channels. Not supported more
	//  float green_thresh; // Reduces maze artifacts produced by bad balance of green channels. Not supported more
	//  int exp_correc;
	//  float exp_shift;
	//  float exp_preser;
	  /* WF debanding */
	//  int wf_debanding;	// code by Yan Vladimirovich is not supported more
	//  float wf_deband_treshold[4];	// code by Yan Vladimirovich is not supported more
	  /* Raw speed */
	//  int use_rawspeed;
	  /* DNG SDK */
	//  int use_dngsdk;
	  /* Disable Auto-scale */
	//  int no_auto_scale;
	  /* Disable intepolation */
	//  int no_interpolation;
	  /*  int x3f_flags; */
	  /* Sony ARW2 digging mode */
	  /* int sony_arw2_options; */
	//  unsigned raw_processing_options;
	//  int sony_arw2_posterization_thr;
	  /* Nikon Coolscan */
	//  float coolscan_nef_gamma;
	//  char p4shot_order[5];
	  /* Custom camera list */
	//  char **custom_camera_strings;
} librawex_output_params_t;

class LibRawEx : public LibRaw
{
	std::unique_ptr<libraw_output_params_t> m_defaultLibrawOutputParamsPtr; // копия исходных параметров Libraw на случай, когда нужно восстановить параметры по умолчанию
public:
	LibRawEx();

	librawex_output_params_t exparams;
	float auto_mul[4]; // рассчитывается в потоке (color.pre_mul при params.use_auto_wb=1)

	std::array<float, 4> getAutoWB();

	// demosaic pack
	void set_pre_interpolate_cb(process_step_callback cb, bool def_cb = false);
	void set_interpolate_bayer_cb(process_step_callback cb, bool def_cb = false);
	void set_post_interpolate_cb(process_step_callback cb, bool def_cb = false);
	void pre_interpolate();
	void interpolate();
	void post_interpolate();
	void ahd_interpolate_mod();
	void afd_interpolate_pl(int afd_passes, int clip_on);
	void ahd_partial_interpolate(int threshold_value);
	void vcd_interpolate(int ahd_cutoff);
	void refinement();
	void es_median_filter();
	void median_filter_new();
	void lmmse_interpolate(int gamma_apply);
	void amaze_demosaic_RT();
	void CA_correct_RT(float cared, float cablue);
	void cfa_linedn(float linenoise);
	void cfa_impulse_gauss(float lclean, float cclean);

	// WF filtering
	int wf_remove_banding();
	double wf_filter_energy(int r1_greenmode, int r1, int r2_greenmode, int r2);
	void wf_bayer4_green_blur(int mode, void* src_image, int src_imgmode, void* dst_image, int dst_imgmode);
	void wf_bayer4_igauss_filter(int radius, void* src_image, int src_imgmode, void* dst_image, int dst_imgmode);
	void wf_bayer4_block_filter(int* radius_list, void* src_image, int src_imgmode, void* dst_image, int dst_imgmode);
};