# include  " esp_camera.h "
# include  < WiFi.h >

//
// AVISO !!! Verifique se você selecionou o ESP32 Wrover Module,
//             ou outra placa com a PSRAM ativada
//
// Selecione o modelo da câmera // Selecione o modelo correto da câmera.
// #define CAMERA_MODEL_WROVER_KIT
// #define CAMERA_MODEL_M5STACK_PSRAM
# define  CAMERA_MODEL_AI_THINKER

const  char * ssid = "Nossa Rede 2016" ; // Digite SSID
const  char * senha = "r68888031120r" ; // Digite a senha


# se definido (CAMERA_MODEL_WROVER_KIT)
# define  PWDN_GPIO_NUM     - 1
# define  RESET_GPIO_NUM    - 1
# define  XCLK_GPIO_NUM     21
# define  SIOD_GPIO_NUM     26
# define  SIOC_GPIO_NUM     27

# define  Y9_GPIO_NUM       35
# define  Y8_GPIO_NUM       34
# define  Y7_GPIO_NUM       39
# define  Y6_GPIO_NUM       36
# define  Y5_GPIO_NUM       19
# define  Y4_GPIO_NUM       18
# define  Y3_GPIO_NUM        5
# define  Y2_GPIO_NUM        4
# define  VSYNC_GPIO_NUM    25
# define  HREF_GPIO_NUM     23
# define  PCLK_GPIO_NUM     22

# elif definido (CAMERA_MODEL_M5STACK_PSRAM)
# define  PWDN_GPIO_NUM      - 1
# define  RESET_GPIO_NUM     15
# define  XCLK_GPIO_NUM      27
# define  SIOD_GPIO_NUM      25
# define  SIOC_GPIO_NUM      23

# define  Y9_GPIO_NUM        19
# define  Y8_GPIO_NUM        36
# define  Y7_GPIO_NUM        18
# define  Y6_GPIO_NUM        39
# define  Y5_GPIO_NUM         5
# define  Y4_GPIO_NUM        34
# define  Y3_GPIO_NUM        35
# define  Y2_GPIO_NUM        32
# define  VSYNC_GPIO_NUM     22
# define  HREF_GPIO_NUM      26
# define  PCLK_GPIO_NUM      21

# elif definido (CAMERA_MODEL_AI_THINKER)
# define  PWDN_GPIO_NUM      32
# define  RESET_GPIO_NUM     - 1
# define  XCLK_GPIO_NUM       0
# define  SIOD_GPIO_NUM      26
# define  SIOC_GPIO_NUM      27

# define  Y9_GPIO_NUM        35
# define  Y8_GPIO_NUM        34
# define  Y7_GPIO_NUM        39
# define  Y6_GPIO_NUM        36
# define  Y5_GPIO_NUM        21
# define  Y4_GPIO_NUM        19
# define  Y3_GPIO_NUM        18
# define  Y2_GPIO_NUM         5
# define  VSYNC_GPIO_NUM     25
# define  HREF_GPIO_NUM      23
# define  PCLK_GPIO_NUM      22

# else
# error "Modelo da câmera não selecionado"
# endif

void  startCameraServer ();

 configuração nula () {
  Serial. begin ( 115200 );
  Serial. setDebugOutput ( true );
  Serial. println ();
  pinMode ( 12 , SAÍDA);        // Inicialize o IO12 no AI-Thinker
  configuração camera_config_t ;
  config. ledc_channel = LEDC_CHANNEL_0;
  config. ledc_timer = LEDC_TIMER_0;
  config. pin_d0 = Y2_GPIO_NUM;
  config. pin_d1 = Y3_GPIO_NUM;
  config. pin_d2 = Y4_GPIO_NUM;
  config. pin_d3 = Y5_GPIO_NUM;
  config. pin_d4 = Y6_GPIO_NUM;
  config. pin_d5 = Y7_GPIO_NUM;
  config. pin_d6 = Y8_GPIO_NUM;
  config. pin_d7 = Y9_GPIO_NUM;
  config. pin_xclk = XCLK_GPIO_NUM;
  config. pin_pclk = PCLK_GPIO_NUM;
  config. pin_vsync = VSYNC_GPIO_NUM;
  config. pin_href = HREF_GPIO_NUM;
  config. pin_sscb_sda = SIOD_GPIO_NUM;
  config. pin_sscb_scl = SIOC_GPIO_NUM;
  config. pin_pwdn = PWDN_GPIO_NUM;
  config. pin_reset = RESET_GPIO_NUM;
  config. xclk_freq_hz = 20000000 ;
  config. pixel_format = PIXFORMAT_JPEG;
  // init com especificações altas para pré-alocar buffers maiores
  if ( psramFound ()) {
    config. frame_size = FRAMESIZE_UXGA;
    config. jpeg_quality = 10 ;
    config. fb_count = 2 ;
  } mais {
    config. frame_size = FRAMESIZE_SVGA;
    config. jpeg_quality = 12 ;
    config. fb_count = 1 ;
  }

  // init da câmera
  esp_err_t err = esp_camera_init (& config);
  if (err! = ESP_OK) {
    Serial. printf ( " O init da câmera falhou com erro 0x% x " , err);
    retorno ;
  }

  // tamanho do quadro suspenso para maior taxa de quadros inicial
  sensor_t * s = esp_camera_sensor_get ();
  s-> set_framesize (s, FRAMESIZE_QVGA);

  Wi-fi. begin (ssid, senha);

  while (WiFi. status ()! = WL_CONNECTED) {
    atraso ( 500 );
    Serial. print ( " . " );
  }
  Serial. println ( " " );
  Serial. println ( " WiFi conectado " );

  startCameraServer ();

  Serial. print ( " Câmera pronta! Use 'http: // " );
  Serial. print (WiFi. localIP ());
  Serial. println ( " 'para conectar " );
}

 loop vazio () {
  // coloque seu código principal aqui, para executar repetidamente:
  atraso ( 10000 );
}
