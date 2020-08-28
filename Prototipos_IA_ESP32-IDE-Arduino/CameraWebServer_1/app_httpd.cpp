// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licenciado sob a licença Apache, versão 2.0 (a "Licença");
// você não pode usar este arquivo, exceto em conformidade com a Licença.
// Você pode obter uma cópia da licença em
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// A menos que exigido pela lei aplicável ou acordado por escrito, o software
// distribuído sob a licença é distribuído "COMO ESTÁ",
// SEM GARANTIAS OU CONDIÇÕES DE QUALQUER TIPO, expressas ou implícitas.
// Veja a licença para o idioma específico que rege as permissões e
// limitações sob a licença.
# include  " esp_http_server.h "
# include  " esp_timer.h "
# include  " esp_camera.h "
# include  " img_converters.h "
# include  " camera_index.h "
# include  " Arduino.h "

# include  " fb_gfx.h "
# include  " fd_forward.h "
# include  " dl_lib.h "
# include  " fr_forward.h "

# define  ENROLL_CONFIRM_TIMES  5
# define  FACE_ID_SAVE_NUMBER  7

# define  FACE_COLOR_WHITE   0x00FFFFFF
# define  FACE_COLOR_BLACK   0x00000000
# define  FACE_COLOR_RED     0x000000FF
# define  FACE_COLOR_GREEN   0x0000FF00
# Definir  FACE_COLOR_BLUE    0x00FF0000
# define  FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
# define  FACE_COLOR_CYAN    (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
# define  FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

typedef  struct {
        size_t tamanho; // número de valores usados ​​para filtrar
        índice size_t ; // índice do valor atual
        size_t count; // contagem de valores
        int sum;
        valores int *; // matriz a ser preenchida com valores
} ra_filter_t ;

typedef  struct {
        httpd_req_t * req;
        size_t len;
} jpg_chunking_t ;

# define  PART_BOUNDARY  " 123456789000000000000987654321 "
 const  char estático * _STREAM_CONTENT_TYPE = " multipart / x-mixed-replace; boundary = " PART_BOUNDARY;
 const  char estático * _STREAM_BOUNDARY = " \ r \ n - " PART_BOUNDARY " \ r \ n " ;
static  const  char * _STREAM_PART = " Tipo de conteúdo: image / jpeg \ r \ n Comprimento do conteúdo:% u \ r \ n \ r \ n " ;

estático  ra_filter_t ra_filter;
httpd_handle_t stream_httpd = NULL ;
httpd_handle_t camera_httpd = NULL ;

static  mtmn_config_t mtmn_config = { 0 };
static  int8_t detection_enabled = 0 ;
static  int8_t recognition_enabled = 0 ;
estático  int8_t is_enrolling = 0 ;
static face_id_list id_list = { 0 };

estático  ra_filter_t * ra_filter_init ( ra_filter_t * filtro, size_t sample_size) {
    memset (filtro, 0 , sizeof ( ra_filter_t ));

    filtro-> valores = ( int *) malloc (SAMPLE_SIZE * sizeof ( int ));
    if (! filter-> values ) {
        return  NULL ;
    }
    memset (filter-> values , 0 , sample_size * sizeof ( int ));

    filter-> size = sample_size;
    filtro de retorno ;
}

static  int  ra_filter_run ( filtro ra_filter_t *, valor int ) {
    if (! filter-> values ) {
        valor de retorno ;
    }
    filter-> sum - = filter-> values [filter-> index ];
    filter-> values [filter-> index ] = valor;
    filter-> sum + = filter-> values [filtro-> índice ];
    filter-> index ++;
    filter-> index = filter-> index % filter-> size ;
    if (filter-> count <filter-> size ) {
        filter-> count ++;
    }
    return filter-> sum / filter-> count ;
}

static  void  rgb_print ( dl_matrix3du_t * image_matrix, cor uint32_t , const  char * str) {
    fb_data_t fb;
    fb. width = imagem_matriz-> w ;
    fb. height = image_matrix-> h ;
    fb. data = image_matrix-> item ;
    fb. bytes_per_pixel = 3 ;
    fb. formato = FB_BGR888;
    fb_gfx_print (& fb, ( largura fb - ( strlen (str) * 14 )) / 2 , 10 , cor, str);
}

static  int  rgb_printf ( dl_matrix3du_t * image_matrix, cor uint32_t , formato const  char *, ...) {
    char loc_buf [ 64 ];
    char * temp = loc_buf;
    int len;
    va_list arg;
    cópia va_list ;
    va_start (arg, formato);
    va_copy (cópia, arg);
    len = vsnprintf (loc_buf, sizeof (loc_buf), formato, arg);
    va_end (cópia);
    if (len> = sizeof (loc_buf)) {
        temp = ( char *) malloc (len + 1 );
        if (temp == NULL ) {
            retornar  0 ;
        }
    }
    vsnprintf (temp, len + 1 , formato, arg);
    va_end (arg);
    rgb_print (imagem_matriz, cor, temperatura);
    if (len> 64 ) {
        livre (temp);
    }
    retorno len;
}

estático  vazio  draw_face_boxes ( dl_matrix3du_t * image_matrix, box_array_t * boxes, int face_id) {
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_YELLOW;
    if (face_id < 0 ) {
        color = FACE_COLOR_RED;
    } Else  se (face_id> 0 ) {
        color = FACE_COLOR_GREEN;
    }
    fb_data_t fb;
    fb. width = imagem_matriz-> w ;
    fb. height = image_matrix-> h ;
    fb. data = image_matrix-> item ;
    fb. bytes_per_pixel = 3 ;
    fb. formato = FB_BGR888;
    para (i = 0 ; i <caixas-> len ; i ++) {
        // caixa retangular
        x = ( int ) caixas-> caixa [i]. box_p [ 0 ];
        y = ( int ) caixas-> caixa [i]. box_p [ 1 ];
        w = ( int ) caixas-> caixa [i]. caixa_p [ 2 ] - x + 1 ;
        h = ( int ) caixas-> caixa [i]. caixa_p [ 3 ] - y + 1 ;
        fb_gfx_drawFastHLine (& fb, x, y, w, cor);
        fb_gfx_drawFastHLine (& fb, x, y + h- 1 , w, cor);
        fb_gfx_drawFastVLine (& fb, x, y, h, cor);
        fb_gfx_drawFastVLine (& fb, x + w- 1 , y, h, cor);
# if  0
        // ponto de referência
        int x0, y0, j;
        para (j = 0; j <10; j + = 2) {
            x0 = (int) caixas-> ponto de referência [i] .landmark_p [j];
            y0 = (int) caixas-> ponto de referência [i] .landmark_p [j + 1];
            fb_gfx_fillRect (& fb, x0, y0, 3, 3, cor);
        }
# endif
    }
}

static  int  run_face_recognition ( dl_matrix3du_t * image_matrix, box_array_t * net_boxes) {
    dl_matrix3du_t * align_face = NULL ;
    int matched_id = 0 ;

    face_alinhada = dl_matrix3du_alloc ( 1 , FACE_WIDTH, FACE_HEIGHT, 3 );
    if (! align_face) {
        Serial. println ( " Não foi possível alocar o buffer de reconhecimento de face " );
        return matched_id;
    }
    if ( align_face (net_boxes, image_matrix, align_face) == ESP_OK) {
        if (is_enrolling == 1 ) {
            int8_t left_sample_face = registration_face (& id_list, align_face);

            if (left_sample_face == (ENROLL_CONFIRM_TIMES - 1 )) {
                Serial. printf ( " Registrando ID da face:% d \ n " , id_list. tail );
            }
            Serial. printf ( " Registrando ID da face:% d amostra% d \ n " , id_list. tail , ENROLL_CONFIRM_TIMES - left_sample_face);
            rgb_printf (image_matrix, FACE_COLOR_CYAN, " ID [% u] Amostra [% u] " , id_list. tail , ENROLL_CONFIRM_TIMES - left_sample_face);
            if (left_sample_face == 0 ) {
                is_enrolling = 0 ;
                Serial. printf ( " ID do rosto registrado :% d \ n " , id_list. tail );
                
            }
        } mais {
            matched_id = reconhece_face (& id_list, align_face);
            if (id_comparado> = 0 ) {
                Serial. printf ( " Corresponder ao ID da face:% u \ n " , par_id);
                rgb_printf (imagem_matriz, FACE_COLOR_GREEN, " Olá assunto% u " , id_comparado);
                digitalWrite ( 12 , ALTO);       // lógica para IO12 / dependente da placa
            } mais {
                Serial. println ( " Nenhuma correspondência encontrada " );
                rgb_print (imagem_matriz, FACE_COLOR_RED, " Alerta de intrusão ! " );
                ID de correspondência = - 1 ;
                digitalWrite ( 12 , BAIXO);        // lógica para IO12 / dependente da placa
            }
        }
    } mais {
        Serial. println ( " Rosto não alinhado " );
        // rgb_print (image_matrix, FACE_COLOR_YELLOW, "Detectado por humanos");
    }

    dl_matrix3du_free (face_alinhada);
    return matched_id;
}

static  size_t  jpg_encode_stream ( void * arg, índice size_t , dados const  void *, size_t len) {
    jpg_chunking_t * j = ( jpg_chunking_t *) arg;
    if (! index ) {
        j-> len = 0 ;
    }
    if ( httpd_resp_send_chunk (j-> req , ( const  char *) dados, len)! = ESP_OK) {
        retornar  0 ;
    }
    j-> len + = len;
    retorno len;
}

estático  esp_err_t  capture_handler ( httpd_req_t * req) {
    camera_fb_t * fb = NULL ;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time ();

    fb = esp_camera_fb_get ();
    if (! fb) {
        Serial. println ( " Falha na captura da câmera " );
        httpd_resp_send_500 (req);
        return ESP_FAIL;
    }

    httpd_resp_set_type (req, " imagem / jpeg " );
    httpd_resp_set_hdr (req, " Disposição de conteúdo " , " embutido; nome do arquivo = capture.jpg " );

    size_t out_len, out_width, out_height;
    uint8_t * out_buf;
    bool s;
    bool detectado = false ;
    int face_id = 0 ;
    if (! detection_enabled || fb-> width > 400 ) {
        size_t fb_len = 0 ;
        if (fb-> format == PIXFORMAT_JPEG) {
            fb_len = fb-> len ;
            res = httpd_resp_send (req, ( const  char *) fb-> buf , fb-> len );
        } mais {
            jpg_chunking_t jchunk = {req, 0 };
            res = frame2jpg_cb (fb, 80 , jpg_encode_stream e jchunk)? ESP_OK: ESP_FAIL;
            httpd_resp_send_chunk (req, NULL , 0 );
            fb_len = jchunk. len ;
        }
        esp_camera_fb_return (fb);
        int64_t fr_end = esp_timer_get_time ();
        Serial. printf ( " JPG:% uB% ums \ n " , ( uint32_t ) (fb_len), ( uint32_t ) ((fr_end - fr_start) / 1000 ));
        return res;
    }

    dl_matrix3du_t * image_matrix = dl_matrix3du_alloc ( 1 , fb-> largura , fb-> altura , 3 );
    if (! image_matrix) {
        esp_camera_fb_return (fb);
        Serial. println ( " dl_matrix3du_alloc falhou " );
        httpd_resp_send_500 (req);
        return ESP_FAIL;
    }

    out_buf = image_matrix-> item ;
    out_len = fb-> largura * fb-> altura * 3 ;
    out_width = fb-> width ;
    out_height = fb-> height ;

    s = fmt2rgb888 ( fb- > buf , fb-> len , fb-> formato , out_buf);
    esp_camera_fb_return (fb);
    if (! s) {
        dl_matrix3du_free (image_matrix);
        Serial. println ( " falhou rgb888 " );
        httpd_resp_send_500 (req);
        return ESP_FAIL;
    }

    box_array_t * net_boxes = face_detect (imagem_matriz, & mtmn_config);

    if (net_boxes) {
        detectado = verdadeiro ;
        if (recognition_enabled) {
            face_id = run_face_recognition (imagem_matriz, caixas de rede);
        }
        draw_face_boxes (image_matrix, net_boxes, face_id);
        livre (net_boxes-> box );
        livre (net_boxes-> marco );
        grátis (net_boxes);
    }

    jpg_chunking_t jchunk = {req, 0 };
    s = fmt2jpg_cb (out_buf, out_len, out_width, out_height, PIXFORMAT_RGB888, 90 , jpg_encode_stream e jchunk);
    dl_matrix3du_free (image_matrix);
    if (! s) {
        Serial. println ( " Falha na compactação JPEG " );
        return ESP_FAIL;
    }

    int64_t fr_end = esp_timer_get_time ();
    Serial. printf ( " FACE:% uB% ums% s% d \ n " , ( uint32_t ) (jchunk. len ), ( uint32_t ) ((fr_end - fr_start) / 1000 ), detectado? " DETECTED " : " " , face_id) ;
    return res;
}

estática  esp_err_t  stream_handler ( httpd_req_t * req) {
    camera_fb_t * fb = NULL ;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0 ;
    uint8_t * _jpg_buf = NULL ;
    char * part_buf [ 64 ];
    dl_matrix3du_t * image_matrix = NULL ;
    bool detectado = false ;
    int face_id = 0 ;
    int64_t fr_start = 0 ;
    int64_t fr_ready = 0 ;
    int64_t fr_face = 0 ;
    int64_t fr_recognize = 0 ;
    int64_t fr_encode = 0 ;

    estático  int64_t last_frame = 0 ;
    if (! last_frame) {
        last_frame = esp_timer_get_time ();
    }

    res = httpd_resp_set_type (req, _STREAM_CONTENT_TYPE);
    if (res! = ESP_OK) {
        return res;
    }

    while ( true ) {
        detectado = falso ;
        face_id = 0 ;
        fb = esp_camera_fb_get ();
        if (! fb) {
            Serial. println ( " Falha na captura da câmera " );
            res = ESP_FAIL;
        } mais {
            fr_start = esp_timer_get_time ();
            fr_ready = fr_start;
            fr_face = fr_start;
            fr_encode = fr_start;
            fr_recognize = fr_start;
            if (! detection_enabled || fb-> width > 400 ) {
                if ( formato fb- > ! = PIXFORMAT_JPEG) {
                    bool jpeg_converted = frame2jpg (fb, 80 , & _jpg_buf, & _jpg_buf_len);
                    esp_camera_fb_return (fb);
                    fb = NULL ;
                    if (! jpeg_converted) {
                        Serial. println ( " Falha na compactação JPEG " );
                        res = ESP_FAIL;
                    }
                } mais {
                    _jpg_buf_len = fb-> len ;
                    _jpg_buf = fb-> buf ;
                }
            } mais {

                image_matrix = dl_matrix3du_alloc ( 1 , fb-> largura , fb-> altura , 3 );

                if (! image_matrix) {
                    Serial. println ( " dl_matrix3du_alloc falhou " );
                    res = ESP_FAIL;
                } mais {
                    if (! fmt2rgb888 ( fb- > buf , fb-> len , fb-> formato , image_matrix-> item )) {
                        Serial. println ( " fmt2rgb888 falhou " );
                        res = ESP_FAIL;
                    } mais {
                        fr_ready = esp_timer_get_time ();
                        box_array_t * net_boxes = NULL ;
                        if (detection_enabled) {
                            net_boxes = face_detect (imagem_matriz, & mtmn_config);
                        }
                        fr_face = esp_timer_get_time ();
                        fr_recognize = fr_face;
                        if (net_boxes || fb-> format ! = PIXFORMAT_JPEG) {
                            if (net_boxes) {
                                detectado = verdadeiro ;
                                if (recognition_enabled) {
                                    face_id = run_face_recognition (imagem_matriz, caixas de rede);
                                }
                                fr_recognize = esp_timer_get_time ();
                                draw_face_boxes (image_matrix, net_boxes, face_id);
                                livre (net_boxes-> box );
                                livre (net_boxes-> marco );
                                grátis (net_boxes);
                            }
                            if (! fmt2jpg (imagem_matriz-> item , fb-> largura * fb-> altura * 3 , fb-> largura , fb-> altura , PIXFORMAT_RGB888, 90 , & _jpg_buf e _jpg_buf_len)) {
                                Serial. println ( " fmt2jpg falhou " );
                                res = ESP_FAIL;
                            }
                            esp_camera_fb_return (fb);
                            fb = NULL ;
                        } mais {
                            _jpg_buf = fb-> buf ;
                            _jpg_buf_len = fb-> len ;
                        }
                        fr_encode = esp_timer_get_time ();
                    }
                    dl_matrix3du_free (image_matrix);
                }
            }
        }
        if (res == ESP_OK) {
            size_t hlen = snprintf (( char *) part_buf, 64 , _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk (req, ( const  char *) part_buf, hlen);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk (req, ( const  char *) _ jpg_buf, _jpg_buf_len);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk (req, _STREAM_BOUNDARY, strlen (_STREAM_BOUNDARY));
        }
        if (fb) {
            esp_camera_fb_return (fb);
            fb = NULL ;
            _jpg_buf = NULL ;
        } caso contrário,  se (_jpg_buf) {
            livre (_jpg_buf);
            _jpg_buf = NULL ;
        }
        if (res! = ESP_OK) {
            quebrar ;
        }
        int64_t fr_end = esp_timer_get_time ();

        int64_t ready_time = (fr_ready - fr_start) / 1000 ;
        int64_t face_time = (fr_face - fr_ready) / 1000 ;
        int64_tknow_time = (fr_recognize - fr_face) / 1000 ;
        int64_t encode_time = (fr_encode - fr_recognize) / 1000 ;
        int64_t process_time = (fr_encode - fr_start) / 1000 ;
        
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time / = 1000 ;
        uint32_t avg_frame_time = ra_filter_run (& ra_filter, frame_time);
        Serial. printf ( " MJPG:% uB% ums (% .1ffps), AVG:% ums (% .1ffps),% u +% u +% u +% u =% u% s% d \ n " ,
            ( uint32_t ) (_ jpg_buf_len),
            ( uint32_t ) frame_time, 1000.0 / ( uint32_t ) frame_time,
            avg_frame_time, 1000.0 / avg_frame_time,
            ( uint32_t ) horário_do_pronto, ( uint32_t ) horário_de_ rosto, ( uint32_t ) reconhecimento_tempo, ( uint32_t ) horário_de_codificação, ( uint32_t ) processo_time,
            (detectou)? " DETECTED " : " " , face_id
        );
    }

    last_frame = 0 ;
    return res;
}

estático  esp_err_t  cmd_handler ( httpd_req_t * req) {
    char * buf;
    size_t buf_len;
    variável char [ 32 ] = { 0 ,};
    valor do caractere [ 32 ] = { 0 ,};

    buf_len = httpd_req_get_url_query_len (req) + 1 ;
    if (buf_len> 1 ) {
        buf = ( char *) malloc (buf_len);
        if (! buf) {
            httpd_resp_send_500 (req);
            return ESP_FAIL;
        }
        if ( httpd_req_get_url_query_str (req, buf, buf_len) == ESP_OK) {
            if ( httpd_query_key_value (buf, " var " , variável, sizeof (variable)) == ESP_OK &&
                httpd_query_key_value (buf, " val " , valor, sizeof (value)) == ESP_OK) {
            } mais {
                livre (buf);
                httpd_resp_send_404 (req);
                return ESP_FAIL;
            }
        } mais {
            livre (buf);
            httpd_resp_send_404 (req);
            return ESP_FAIL;
        }
        livre (buf);
    } mais {
        httpd_resp_send_404 (req);
        return ESP_FAIL;
    }

    int val = atoi (valor);
    sensor_t * s = esp_camera_sensor_get ();
    int res = 0 ;

    if (! strcmp (variável, " tamanho do quadro " )) {
        if (s-> pixformat == PIXFORMAT_JPEG) res = s-> set_framesize (s, ( framesize_t ) val);
    }
     caso contrário, se (! strcmp (variável, " qualidade " )) res = s-> set_quality (s, val);
     caso contrário, se (! strcmp (variável, " contraste " )) res = s-> set_contrast (s, val);
     caso contrário, if (! strcmp (variável, " brilho " )) res = s-> set_brightness (s, val);
     caso contrário, se (! strcmp (variável, " saturação " )) res = s-> set_saturation (s, val);
     caso contrário, se (! strcmp (variável, " gainceiling " )) res = s-> set_gainceiling (s, ( gainceiling_t ) val);
     caso contrário, se (! strcmp (variável, " barra de cores " )) res = s-> set_colorbar (s, val);
     caso contrário, se (! strcmp (variável, " awb " )) res = s-> set_whitebal (s, val);
     caso contrário, se (! strcmp (variável, " agc " )) res = s-> set_gain_ctrl (s, val);
     caso contrário, se (! strcmp (variável, " aec " )) res = s-> set_exposure_ctrl (s, val);
     caso contrário, se (! strcmp (variável, " hmirror " )) res = s-> set_hmirror (s, val);
     caso contrário, se (! strcmp (variável, " vflip " )) res = s-> set_vflip (s, val);
     caso contrário, se (! strcmp (variável, " awb_gain " )) res = s-> set_awb_gain (s, val);
     caso contrário, se (! strcmp (variável, " agc_gain " )) res = s-> set_agc_gain (s, val);
     caso contrário, se (! strcmp (variável, " valor_aec " )) res = s-> set_aec_value (s, val);
     caso contrário, se (! strcmp (variável, " aec2 " )) res = s-> set_aec2 (s, val);
     caso contrário, se (! strcmp (variável, " dcw " )) res = s-> set_dcw (s, val);
     caso contrário, se (! strcmp (variável, " bpc " )) res = s-> set_bpc (s, val);
     caso contrário, se (! strcmp (variável, " wpc " )) res = s-> set_wpc (s, val);
     caso contrário, se (! strcmp (variável, " raw_gma " )) res = s-> set_raw_gma (s, val);
     caso contrário, se (! strcmp (variável, " lenc " )) res = s-> set_lenc (s, val);
     caso contrário, se (! strcmp (variável, " efeito_específico " )) res = s-> set_especial_effect (s, val);
     caso contrário, se (! strcmp (variável, " wb_mode " )) res = s-> set_wb_mode (s, val);
     caso contrário, if (! strcmp (variável, " ae_level " )) res = s-> set_ae_level (s, val);
     caso contrário, if (! strcmp (variável, " face_detect " )) {
        detecção_ativada = val;
        if (! detection_enabled) {
            reconhecimento_ ativado = 0 ;
        }
    }
     caso contrário, se (! strcmp (variável, " face_enroll " )) is_enrolling = val;
     caso contrário, if (! strcmp (variável, " face_recognize " )) {
        reconhecimento_ativado = val;
        if (recognition_enabled) {
            detecção_ativada = val;
        }
    }
    mais {
        res = -1 ;
    }

    se (res) {
        retornar  httpd_resp_send_500 (req);
    }

    httpd_resp_set_hdr (req, " Access-Control-Allow-Origin " , " * " );
    retornar  httpd_resp_send (req, NULL , 0 );
}

 manipulador de  status esp_err_t estático ( httpd_req_t * req) {
     char estático json_response [ 1024 ];

    sensor_t * s = esp_camera_sensor_get ();
    char * p = json_response;
    * p ++ = ' { ' ;

    p + = sprintf (p, " \" tamanho da moldura \ " :% u, " , s-> status . tamanho da moldura );
    p + = sprintf (p, " \" qualidade \ " :% u, " , s-> status . qualidade );
    p + = sprintf (p, " \" brilho \ " :% d, " , s-> status . brilho );
    p + = sprintf (p, " \" contraste \ " :% d, " , s-> status . contraste );
    p + = sprintf (p, " \" saturação \ " :% d, " , s-> status . saturação );
    p + = sprintf (p, " \" efeito_específico \ " :% u " , s-> status . efeito_específico );
    p + = sprintf (p, " \" modo wb_ "" :% u "" , s-> status . modo wb );
    p + = sprintf (p, " \" awb \ " :% u, " , s-> status . awb );
    p + = sprintf (p, " \" awb_gain \ " :% u " , s-> status . awb_gain );
    p + = sprintf (p, " \" aec \ " :% u, " , s-> status . aec );
    p + = sprintf (p, " \" aec2 \ " :% u, " , s-> status . aec2 );
    p + = sprintf (p, " \" ae_level \ " :% d, " , s-> status . ae_level );
    p + = sprintf (p, " \" aec_value \ " :% u, " , s-> status . aec_value );
    p + = sprintf (p, " \" agc \ " :% u, " , s-> status . agc );
    p + = sprintf (p, " \" agc_gain \ " :% u " , s-> status . agc_gain );
    p + = sprintf (p, " \" gainceiling \ " :% u, " , s-> status . gainceiling );
    p + = sprintf (p, " \" bpc \ " :% u, " , s-> status . bpc );
    p + = sprintf (p, " \" wpc \ " :% u, " , s-> status . wpc );
    p + = sprintf (p, " \" raw_gma \ " :% u " , s-> status . raw_gma );
    p + = sprintf (p, " \" lenc \ " :% u, " , s-> status . lenc );
    p + = sprintf (p, " \" vflip \ " :% u, " , s-> status . vflip );
    p + = sprintf (p, " \" hmirror \ " :% u, " , s-> status . hmirror );
    p + = sprintf (p, " \" dcw \ " :% u, " , s-> status . dcw );
    p + = sprintf (p, " \" barra de cores \ " :% u, " , s-> status . barra de cores );
    p + = sprintf (p, " \" detecção de face \ " :% u, " , detecção_ativada);
    p + = sprintf (p, " \" face_enroll \ " :% u " , is_enrolling);
    p + = sprintf (p, " \" face_recognize \ " :% u " , reconhecimento_ ativado );
    * p ++ = ' } ' ;
    * p ++ = 0 ;
    httpd_resp_set_type (req, " application / json " );
    httpd_resp_set_hdr (req, " Access-Control-Allow-Origin " , " * " );
    retornar  httpd_resp_send (req, json_response, strlen (json_response));
}

estático  esp_err_t  index_handler ( httpd_req_t * req) {
    httpd_resp_set_type (req, " text / html " );
    httpd_resp_set_hdr (req, " Codificação de Conteúdo " , " gzip " );
    retornar  httpd_resp_send (req, ( const  char *) index_html_gz, index_html_gz_len);
}

void  startCameraServer () {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG ();

    httpd_uri_t index_uri = {
        . uri        = " / " ,
        . método     = HTTP_GET,
        . manipulador    = index_handler,
        . user_ctx   = NULL
    };

    httpd_uri_t status_uri = {
        . uri        = " / status " ,
        . método     = HTTP_GET,
        . manipulador    = status_handler,
        . user_ctx   = NULL
    };

    httpd_uri_t cmd_uri = {
        . uri        = " / controle " ,
        . método     = HTTP_GET,
        . manipulador    = cmd_handler,
        . user_ctx   = NULL
    };

    httpd_uri_t capture_uri = {
        . uri        = " / captura " ,
        . método     = HTTP_GET,
        . manipulador    = capture_handler,
        . user_ctx   = NULL
    };

   httpd_uri_t stream_uri = {
        . uri        = " / stream " ,
        . método     = HTTP_GET,
        . manipulador    = stream_handler,
        . user_ctx   = NULL
    };


    ra_filter_init (& ra_filter, 20 );
    
    mtmn_config. min_face = 80 ;
    mtmn_config. pirâmide = 0,7 ;
    mtmn_config. p_threshold . pontuação = 0,6 ;
    mtmn_config. p_threshold . ms = 0,7 ;
    mtmn_config. r_threshold . pontuação = 0,7 ;
    mtmn_config. r_threshold . ms = 0,7 ;
    mtmn_config. r_threshold . candidato_número = 4 ;
    mtmn_config. o_threshold . pontuação = 0,7 ;
    mtmn_config. o_threshold . ms = 0,4 ;
    mtmn_config. o_threshold . candidato_número = 1 ;
    
    face_id_init (& id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
    
    Serial. printf ( " Iniciando o servidor da web na porta: '% d' \ n " , config. server_port );
    if ( httpd_start (& camera_httpd, & config) == ESP_OK) {
        httpd_register_uri_handler (camera_httpd, & index_uri);
        httpd_register_uri_handler (camera_httpd, & cmd_uri);
        httpd_register_uri_handler (camera_httpd, & status_uri);
        httpd_register_uri_handler (camera_httpd, & capture_uri);
    }

    config. server_port + = 1 ;
    config. ctrl_port + = 1 ;
    Serial. printf ( " Iniciando servidor de stream na porta: '% d' \ n " , config. server_port );
    if ( httpd_start (& stream_httpd, & config) == ESP_OK) {
        httpd_register_uri_handler (stream_httpd, & stream_uri);
    }
}
