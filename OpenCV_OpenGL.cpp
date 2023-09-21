
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <chrono>
#include <iostream>




using namespace cv;
using namespace std;
using namespace chrono;


cv::VideoCapture cap;

cv::Mat src;

bool cuadrado = true;

bool dibujarPartitura = true;
bool permiso = false;

int numCirculos;                           // Tamanyo del array de circulos buenos
vector<Vec3f> flauta_referencia;        //Aqui se guarda la posicion de los agujeros de la flauta original

std::chrono::time_point<std::chrono::steady_clock> instanteInicial, instanteFinal;
std::chrono::milliseconds elapsed_time;

int posNotaActual = 0; //Aqui se guarda la posici�n de la nota actual a tocar
int notaTocada = -1;

enum Nota {
    DO1,
    SI,
    LA,
    SOL,
    FA,
    MI,
    RE,
    DO,
    LA_SOST,
    SOL_SOST,
    FA_SOST
};
//Dibuja las lineas del pentagrama
void drawPentagram(cv::Mat* frame) {
    cv::line(*frame, cv::Point(0, 20), cv::Point(640, 20), cv::Scalar(0, 0, 0), 1, LINE_AA);
    cv::line(*frame, cv::Point(0, 40), cv::Point(640, 40), cv::Scalar(0, 0, 0), 1, LINE_AA);
    cv::line(*frame, cv::Point(0, 60), cv::Point(640, 60), cv::Scalar(0, 0, 0), 1, LINE_AA);
    cv::line(*frame, cv::Point(0, 80), cv::Point(640, 80), cv::Scalar(0, 0, 0), 1, LINE_AA);
    cv::line(*frame, cv::Point(0, 100), cv::Point(640, 100), cv::Scalar(0, 0, 0), 1, LINE_AA);
}
void drawMarcaSost(cv::Mat* frame, int y, int x, cv::Scalar color) { //función para dibujar la marca de sostenido
    line(*frame, cv::Point(x - 32, y + 10), cv::Point(x - 12, y + 8), color, 2, LINE_AA);
    line(*frame, cv::Point(x - 32, y - 8), cv::Point(x - 12, y - 10), color, 2, LINE_AA);
    line(*frame, cv::Point(x - 30, y + 12), cv::Point(x - 30, y - 12), color, 2, LINE_AA);
    line(*frame, cv::Point(x - 14, y + 12), cv::Point(x - 14, y - 12), color, 2, LINE_AA);
}
void drawNota(cv::Mat* frame, int y, int x, cv::Scalar color) {
    //cv::circle(*frame, Point(x, y), 10, cv::Scalar(255, 0, 255), -1, LINE_AA);
    cv::circle(*frame, Point(x, y), 10, color, -1, LINE_AA);
}

void drawNotaDo(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 120, x, color);
}

void drawNotaRe(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 110, x, color);
}

void drawNotaMi(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 100, x, color);
}

void drawNotaFa(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 90, x, color);
}

void drawNotaSol(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 80, x, color);
}

void drawNotaLa(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 70, x, color);
}

void drawNotaSi(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 60, x, color);
}

void drawNotaDo1(cv::Mat* frame, int x, cv::Scalar color) {
    drawNota(frame, 50, x, color);
}

void drawNotaLaSost(cv::Mat* frame, int x, cv::Scalar color) {
    /*line(*frame, cv::Point(x - 32, 80), cv::Point(x - 12, 78), color, 2, LINE_AA);
    line(*frame, cv::Point(x-32, 62), cv::Point(x-12, 60), color, 2, LINE_AA);
    line(*frame, cv::Point(x-30, 82), cv::Point(x-30, 58), color, 2, LINE_AA);
    line(*frame, cv::Point(x-14, 82), cv::Point(x-14, 58), color, 2, LINE_AA);*/
    drawMarcaSost(frame, 70, x, color);
    drawNota(frame, 70, x, color);
}

void drawNotaSolSost(cv::Mat* frame, int x, cv::Scalar color) {
    drawMarcaSost(frame, 80, x, color);
    drawNota(frame, 80, x, color);
}

void drawNotaFaSost(cv::Mat* frame, int x, cv::Scalar color) {
    drawMarcaSost(frame, 90, x, color);
    drawNota(frame, 90, x, color);
}

void drawPartitura(cv::Mat* frame, int partitura[], int longPartitura) {
    int x = 50;
    cv::Scalar colorNotaTocada = Scalar(0, 255, 0);//Si la nota ya ha sido tocada se marcar� en verde
    cv::Scalar colorNotaSinTocar = Scalar(0, 0, 0);
    //printf("Notas de la partitura: %d\n", sizeof(partitura));
    for (int i = 0; i < longPartitura; i++) {
        cv::Scalar color = colorNotaTocada;
        if (i >= posNotaActual) color = colorNotaSinTocar;//Se mira si la nota ha sido tocada o no comparando su posici�n en la partitura con la posici�n de la nota a tocar
        int nota = partitura[i];
        switch (nota) {
        case DO:
            //printf("Nota DO");
            drawNotaDo(frame, x, color);
            break;
        case RE:
            //printf("Nota RE");
            drawNotaRe(frame, x, color);
            break;
        case MI:
            //printf("Nota MI");
            drawNotaMi(frame, x, color);
            break;
        case FA:
            //printf("Nota FA");
            drawNotaFa(frame, x, color);
            break;
        case SOL:
            //printf("Nota SOL");
            drawNotaSol(frame, x, color);
            break;
        case LA:
            //printf("Nota LA");
            drawNotaLa(frame, x, color);
            break;
        case SI:
            //printf("Nota SI");
            drawNotaSi(frame, x, color);
            break;
        case DO1:
            //printf("Nota DO AGUDO");
            drawNotaDo1(frame, x, color);
            break;
        case LA_SOST:
            //printf("Nota DO AGUDO");
            drawNotaLaSost(frame, x, color);
            break;
        case SOL_SOST:
            //printf("Nota DO AGUDO");
            drawNotaSolSost(frame, x, color);
            break;
        case FA_SOST:
            //printf("Nota DO AGUDO");
            drawNotaFaSost(frame, x, color);
            break;
        }
        x += 50;
    }
}

void nuevaReferenciaFlauta(vector<Vec3f> flautaTiempoReal) {
    flauta_referencia = flautaTiempoReal;
    for (Vec3f c : flauta_referencia) {
        cout << c << "\n";
    }
}

void printNotaTocada(int nota, cv::Mat* frame) {
    String message = "Estas tocando la nota ";
    switch (nota) {
    case DO:
        //printf("Nota DO");
        message += "DO";
        break;
    case RE:
        //printf("Nota RE");
        message += "RE";
        break;
    case MI:
        //printf("Nota MI");
        message += "MI";
        break;
    case FA:
        //printf("Nota FA");
        message += "FA";
        break;
    case SOL:
        //printf("Nota SOL");
        message += "SOL";
        break;
    case LA:
        //printf("Nota LA");
        message += "LA";
        break;
    case SI:
        //printf("Nota SI");
        message += "SI";
        break;
    case DO1:
        //printf("Nota DO'");
        message += "DO'";
        break;
    case LA_SOST:
        //printf("Nota DO'");
        message += "LA#'";
        break;
    case SOL_SOST:
        //printf("Nota DO'");
        message += "SOL#'";
        break;
    case FA_SOST:
        //printf("Nota DO'");
        message += "SOL#'";
        break;
    default:
        message = "Nota desconocida";
        break;
    }
    cv::putText(*frame, message, Point(400, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 2, LINE_AA, false);
}

void tocandoNota(vector<Vec3f> flautaTiempoReal, int partitura[]) {
    if (permiso) {
        int nota = -1;
        instanteFinal = std::chrono::steady_clock::now();
        elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(instanteFinal - instanteInicial);
        if (elapsed_time.count() >= 1000) {
            if (flautaTiempoReal.size() == 0) {
                nota = 7;
            }
            else {
                float primeraNota = flautaTiempoReal.front()[1];

                for (int i = 0; i < flauta_referencia.size(); i++) {

                    if (primeraNota <= flauta_referencia[i][1] + 20 && primeraNota >= flauta_referencia[i][1] - 20) {
                        if (i == 0 && flautaTiempoReal.size() == 6) {  //CASO DO AGUDO
                            float segundaNota = flautaTiempoReal[1][1];
                            if (segundaNota <= flauta_referencia[i + 2][1] + 20 && segundaNota >= flauta_referencia[i + 2][1] - 20) {
                                nota = i;
                                break;
                            }
                        }
                        else if (i == 1 && flautaTiempoReal.size() == 5) {     //CASO LA SOSTENIDO
                            float segundaNota = flautaTiempoReal[1][1];
                            if (segundaNota <= flauta_referencia[i + 2][1] + 20 && segundaNota >= flauta_referencia[i + 2][1] - 20) {
                                nota = 8;
                                break;
                            }
                        }
                        else if (i == 2 && flautaTiempoReal.size() == 3) {    //CASO SOL SOSTENIDO
                            float segundaNota = flautaTiempoReal[1][1];
                            if (segundaNota <= flauta_referencia[i + 3][1] + 20 && segundaNota >= flauta_referencia[i + 3][1] - 20) {
                                nota = 9;
                                break;
                            }
                        }
                        else if (i == 3 && flautaTiempoReal.size() == 1) {  // CASO FA SOSTENIDO
                            nota = 10;
                            break;
                        }
                        else if (i > 0 && flautaTiempoReal.size() == 7 - i) {
                            nota = i;
                            break;
                        }
                    }
                }
            }

            //cout << "Nota " << nota << "\n";

            //Comparamos la nota identificada con la nota actual, si coinciden, movemos la nota actual
            ;
            if (posNotaActual < 11) {
                if (partitura[posNotaActual] == nota) posNotaActual++;
                
            }
            notaTocada = nota;
            instanteInicial = std::chrono::steady_clock::now();
        }
    }

}

bool sortCircles(Vec3f c1, Vec3f c2) {

    return(c1[1] < c2[1]);
}

void printNota(int partitura[], cv::Mat* frame) {
    if (posNotaActual > 0) {
        switch (partitura[posNotaActual - 1]) {
        case DO:
            //printf("Nota DO");
            cv::putText(*frame, "DO", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case RE:
            //printf("Nota RE");
            cv::putText(*frame, "RE", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case MI:
            //printf("Nota MI");
            cv::putText(*frame, "MI", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case FA:
            //printf("Nota FA");
            cv::putText(*frame, "FA", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case SOL:
            //printf("Nota SOL");
            cv::putText(*frame, "SOL", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case LA:
            //printf("Nota LA");
            cv::putText(*frame, "LA", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case SI:
            //printf("Nota SI");
            cv::putText(*frame, "SI", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case DO1:
            //printf("Nota DO'");
            cv::putText(*frame, "DO'", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case LA_SOST:
            //printf("Nota DO'");
            cv::putText(*frame, "LA#", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case SOL_SOST:
            //printf("Nota DO'");
            cv::putText(*frame, "SOL#", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        case FA_SOST:
            //printf("Nota DO'");
            cv::putText(*frame, "FA#", Point(10, 300), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2, LINE_AA, false);
            break;
        }
    }
}

int main(int argc, char** argv)
{
    //Leer un fotograma de la fuente de video
    cv::Mat frame;

    cv::Mat i_frame;

    cv::Mat opciones;



    numCirculos = 7;

    int X_PANTALLA = 640;
    int Y_PANTALLA = 480;
    int tamanyoCuadradox = 100;
    int tamanyoCuadradoy = 400;
    int x_ref = (X_PANTALLA / 2) - (tamanyoCuadradox / 2);
    int y_ref = 100;
    int distanciaMaxV = 5;

    int centro = x_ref + (tamanyoCuadradox / 2.0);

    int tamanyoMaxCir = 10;
    int tamanyoMinCir = 1;
    int param1 = 200;
    int param2 = 10;
    cap.open(0);
    cap >> i_frame;
    //cv::flip(i_frame, frame, -1);

    bool salir = false;
    namedWindow("Flauta Master", WINDOW_NORMAL);//640 width, 480 height
    namedWindow("Opciones", WINDOW_NORMAL);//Ventana de configuración



    posNotaActual = 0;//Se inicia la posici�n de la nota actual
    //Creamos los distintos Trackbars y los añadimos a la ventana de configuración
    cv::createTrackbar("Tamaño max circulo", "Opciones", &tamanyoMaxCir, 100, 0, 0);
    cv::createTrackbar("Tamaño min circulo", "Opciones", &tamanyoMinCir, 100, 0, 0);
    cv::createTrackbar("Param1", "Opciones", &param1, 1000, 0, 0);
    cv::setTrackbarMin("Param1", "Opciones", 1);
    cv::createTrackbar("Param2", "Opciones", &param2, 1000, 0, 0);
    cv::setTrackbarMin("Param2", "Opciones", 1);

    //Imprimimos las instruciones de la aplicación

    printf("INSTRUCCIONES DE FLAUTA MASTER\n");
    printf("Flauta Master consiste en una aplicacion para aprender a tocar las notas de la flauta, lo unico que necesitas es una flauta y una camara.\n");
    printf("1.USO BASICO\n");
    printf("Coloca la flauta en el centro del rectangulo que aparece en pantalla, cuando todos los agujeros de la flauta hayan sean detectados, pulsa SPACE y, si todo esta correcto, se imprimira un tick verde en pantalla indicando que se ha guardado la referencia de la flauta.\n");
    printf("Trata de mantener la flauta mas o menos en la misma posicion con la que se guardo la referencia para detectar correctamente las notas. \n");
    printf("Ahora, simplemente, toca las notas dejando los dedos sobre la flauta durante unos segundos. Veras como se iran marcando las notas sobre la partitura cuando sean correctas.\n");
    printf("2.TECLAS Y CONFIGURACION\n");
    printf("-Teclas q, Q, ESCAPE. Salir de la aplicacion.\n");
    printf("-Teclas h, H. Muestra u oculta el cuadrado en pantalla.\n");
    printf("-Teclas p, P. Muestra u oculta la partitura en pantalla.\n");
    printf("-Teclas r, R. Reinicia la partitura.\n");
    printf("-Tecla SPACE. Guarda una nueva referencia de la flauta.\n");
    printf("Ventana opciones: Sirve para configurar la deteccion de los agujeros de la flauta.\n");
    printf("-Tamanyo max circulo. Varia el tamanyo maximo de los circulos detectados en el cuadrado.\n");
    printf("-Tamanyo min circulo. Varia el tamanyo minimo de los circulos detectados en el cuadrado.\n");
    printf("-Param1 y Param2. Umbrales de deteccion de circulos. Param1 varia la deteccion de bordes y Param2 varia el umbral acumulador (cuanto menor sea Param2 más círculos se detectaran).\n");

    while (!salir)
    {
        //capture.read( frame );
        if (!cap.grab()) { // Congela la imatge a la c�mera
            salir = true;
            break;
        }
        else {
            cap.retrieve(frame, 0); // Decodes and returns the grabbed video frame.
        }
        int width = cap.get(CAP_PROP_FRAME_WIDTH);
        int height = cap.get(CAP_PROP_FRAME_HEIGHT);
        //printf("Width: %d\n", width);
        //printf("height: %d\n", height);
        if (permiso) {
            cv::line(frame, cv::Point(230, 140), cv::Point(240, 150), cv::Scalar(0, 255, 0), 3, LINE_AA);
            cv::line(frame, cv::Point(240, 150), cv::Point(250, 130), cv::Scalar(0, 255, 0), 3, LINE_AA);
        }
        if (cuadrado) {
            //cv::rectangle(frame, cv::Rect(270, 60, 100, 600), cv::Scalar(255, 0, 255), 3, LINE_AA);
            cv::rectangle(frame, cv::Rect(x_ref, y_ref, tamanyoCuadradox, tamanyoCuadradoy), cv::Scalar(255, 0, 255), 3, LINE_AA);
        }
        int partitura[] = { DO, RE, MI, FA, FA_SOST, SOL, SOL_SOST, LA, LA_SOST, SI, DO1 };
        if (dibujarPartitura) {
            drawPentagram(&frame);

            //Con una distancia de 40 entre el centro de los circulos se visualizan bien las notas
            /*drawNotaDo(&frame, 10);
            drawNotaRe(&frame, 50);
            drawNotaMi(&frame, 90);
            drawNotaFa(&frame, 130);
            drawNotaSol(&frame, 170);
            drawNotaLa(&frame, 210);
            drawNotaSi(&frame, 250);
            drawNotaDo1(&frame, 290);*/
            //cv::circle(frame, Point(0, 0), 1, Scalar(0, 100, 100), 3, LINE_AA);

            //printf("Notas de la partitura: %d\n", (int)(sizeof(partitura) / sizeof(DO)));
            //printf("Tamaño partitura: %d\n", (int)(sizeof(partitura) / sizeof(DO)));
            drawPartitura(&frame, partitura, (int)(sizeof(partitura) / sizeof(DO)));
            printNota(partitura, &frame);
        }

        cv::Mat gray;

        //Detecci�n de c�rculos utilizando HoughCircles de OpenCV
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, gray.rows / 16.0, param1, param2, tamanyoMinCir, tamanyoMaxCir);  //Probar HOUGH_GRADIENT_ALT


        //printf("\nNumero de circulos: %d\n", (int)(circles.size()));

        vector<Vec3f> circles_buenos;


        for (Vec3f cir : circles) {  //Solo detecta los agujeros que esten m�s o menos en el centro
            if (cir[0] <= centro + 15 && cir[0] >= centro - 15 && cir[1] > y_ref && cir[1] < y_ref + tamanyoCuadradoy) {  //Se puede fusionar con el if de arriba?
                circles_buenos.push_back(cir);
                Point center = Point(cir[0], cir[1]);

                // circle center
                circle(frame, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
                // circle outline
                int radius = cir[2];
                circle(frame, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
            }

        }

        sort(circles_buenos.begin(), circles_buenos.end(), sortCircles);

        // for (Vec3f c : circles_buenos)
        //     cout << "[" << c[0]  << ", " << c[1] << "] " << "\n";

        if (numCirculos != circles_buenos.size()) {                    //Si se detecta inconsistencia se resetea tiempo
            instanteInicial = std::chrono::steady_clock::now();
            numCirculos = circles_buenos.size();
            //cout << "Inconsistencia \n";
        }

        tocandoNota(circles_buenos, partitura);
        if (permiso) printNotaTocada(notaTocada, &frame);

        int key = waitKey(25) & 255;  // Espera una tecla los milisegundos que haga falta
        switch (key) {
        case 27:
        case 'q':
        case 'Q':
            cap.release();
            exit(0);
            break;

        case 'h':
            if (cuadrado) {
                cuadrado = false;
            }
            else {
                cuadrado = true;
            }
            break;
        case 'p':
        case 'P'://Se dibuja o borra la partitura
            if (dibujarPartitura) {
                dibujarPartitura = false;
            }
            else {
                dibujarPartitura = true;
            }
            break;
        case 'R':
        case 'r'://Se vuelve a comenzar la partitura
            posNotaActual = 0;
            break;
        case 32:                        //Spacebar
            if (circles_buenos.size() == 7) {
                nuevaReferenciaFlauta(circles_buenos);
                instanteInicial = std::chrono::steady_clock::now();
                permiso = true;
            }
            else { cout << "Numero de circulos no se corresponden con los de una flauta \n"; }
            break;




        }


        imshow("Flauta Master", frame);

    }

    return 0;
}