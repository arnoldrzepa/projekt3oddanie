#include <pybind11/pybind11.h>
#include <vector>
#include <cmath>
#include <matplot/matplot.h>
#include "AudioFile.h"
#include "CImg.h"
#include <fstream>
#include <sstream>
#include <pybind11/stl.h>

using namespace std;


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

std::vector<double> x;
std::vector<double> y;
typedef vector<vector<vector<float>>> Image;
typedef vector<vector<float>> Matrix; 



void function_wave(double amplitude, int sampling_rate, double duration, double freq) {
    
    int n_samples = static_cast<int>(duration * sampling_rate);

    for (int i = 0; i < n_samples; i++) {
        x.push_back((static_cast<double>(i) / sampling_rate));
        y.push_back(amplitude * (std::sin(x[i] * freq * 6.28)) * (std::sin(x[i] * freq * 6.28 * 0.05)) * x[i] * 0.1);
        double buf1 = y[i];

    }
}

void audio(const string path)
{
    
    AudioFile<double> file;
    file.load (path); // Load an audio file

    
    int samplingRate = file.getSampleRate();
    double duration = file.getLengthInSeconds();

    std::vector<float> signal;
    for (int i = 0; i < file.getNumSamplesPerChannel(); ++i) {
        signal.push_back(file.samples[0][i]);
    }

    std::vector<double> time;
    for (int i = 0; i < signal.size(); ++i) {
        time.push_back(i / static_cast<double>(samplingRate));
    }

    matplot::figure();
    matplot::plot(time, signal);
    matplot::xlabel("Czas [s]");
    matplot::ylabel("Amplituda");
    matplot::title("Sygna³ dŸwiêkowy");
    matplot::grid(true);
    matplot::save("wykressampli.jpg");
    matplot::show();
}


Image read(const string& FilePath) {
    ifstream image(FilePath);
    string type, width, height, RGB;
    image >> type >> width >> height >> RGB;

    int W = stoi(width);
    int H = stoi(height);
    int C = 3;

    Image img(H, vector<vector<float>>(W, vector<float>(C)));

    int r, g, b;
    for (int h = 0; h < H; ++h) {
        for (int w = 0; w < W; ++w) {
            image >> r >> g >> b;
            img[h][w][0] = r;
            img[h][w][1] = g;
            img[h][w][2] = b;
        }
    }
    return img;
}

void save(const Image& image, const string& outputFilePath) {
    ofstream newImage(outputFilePath);
    int H = image.size();
    int W = image[0].size();
    int C = image[0][0].size();

    newImage << "P3" << endl;
    newImage << W << " " << H << endl;
    newImage << "255" << endl;

    for (int h = 0; h < H; ++h) {
        for (int w = 0; w < W; ++w) {
            for (int c = 0; c < C; ++c) {
                newImage << static_cast<int>(image[h][w][c]);
                if (c < C - 1) {
                    newImage << " ";
                }
            }
            newImage << endl;
        }
    }
}

Image filter(const Image& image, const Matrix& kernel) {
    int H = image.size();
    int W = image[0].size();
    int C = image[0][0].size();
    int h = kernel.size();
    int w = kernel[0].size();

    Image output(H - h + 1, Matrix(W - w + 1, vector<float>(C, 0.0f)));

    for (int i = 0; i <= H - h; ++i) {
        for (int j = 0; j <= W - w; ++j) {
            for (int kh = 0; kh < h; ++kh) {
                for (int kw = 0; kw < w; ++kw) {
                    for (int c = 0; c < C; ++c) {
                        output[i][j][c] += image[i + kh][j + kw][c] * kernel[kh][kw];
                    }
                }
            }
        }
    }

    return output;
}

void process(const std::string& FilePath, const std::string& outputFilePath, const Matrix& kernel) {
    Image image = read(FilePath);

    Image filteredImage = filter(image, kernel);
    save(filteredImage, outputFilePath);
}
    
void sin_wave(double amplitude, int sampling_rate, double duration, double freq) {
    
    int n_samples = static_cast<int>(duration * sampling_rate);

    for (int i = 0; i < n_samples; i++) {
        x.push_back((static_cast<double>(i) / sampling_rate));
        y.push_back(amplitude * (std::sin(x[i] * freq * 6.28)));
    }
}

void cos_wave(double amplitude, int sampling_rate, double duration, double freq) {
    
    int n_samples = static_cast<int>(duration * sampling_rate);

    for (int i = 0; i < n_samples; i++) {
        x.push_back((static_cast<double>(i) / sampling_rate));
        y.push_back(amplitude * (std::cos(x[i] * freq * 6.28)));
    }
}

void saw_wave(double amplitude, int sampling_rate, double duration, double freq) {
    
    int n_samples = static_cast<int>(duration * sampling_rate);
    int rate_i = static_cast<int>(duration * freq) + 1;

    double buf1 = 0;
    for (int i = 0; i < n_samples; i++) {

        x.push_back((static_cast<double>(i) / sampling_rate));
        y.push_back(amplitude * (x[i] * freq - buf1 - 0.5));

        for (int j = 0; j < rate_i; j++) {
            if (i != 0 && ((i * freq) / sampling_rate) == j) {
                buf1++;
                break;
            }
        }
    }
}

void square_wave(double amplitude, int sampling_rate, double duration, double freq) {
 
    int n_samples = static_cast<int>(duration * sampling_rate);

    for (int i = 0; i < n_samples; i++) {
        x.push_back((static_cast<double>(i) / sampling_rate));
        y.push_back(amplitude * (std::sin(x[i] * freq * 6.28)));
        double buf1 = y[i];
        if (buf1 > 0) {
            y[i] = amplitude / 2;
        }
        else {
            y[i] = -amplitude / 2;
        }
    }

}

// funkcja progujaca, nalezy podac amplitude, czestotliwosc probkowania, czas przebiegu oraz czestotliwosc funkcji wyjsciowej, prog oraz typ funcji wejsiowej (1) 
void wave_threshold(double amplitude, int sampling_rate, double duration, double freq, double threshold, int which_function) {

    int n_samples = static_cast<int>(duration * sampling_rate);

    //(1) wybor funkcji wejsciowej: 1 - sinus, 2 - cosinus, 3 - prostokatna, 4 - pilokszataltna, jakikolwiek inny int - funkcja testowa
    switch (which_function) {
    case 0:
        sin_wave(amplitude, sampling_rate, duration, freq);
        break;
    case 1:
        cos_wave(amplitude, sampling_rate, duration, freq);
        break;
    case 2:
        square_wave(amplitude, sampling_rate, duration, freq);
        break;
    case 3:
        saw_wave(amplitude, sampling_rate, duration, freq);
        break;
    default:
        function_wave(amplitude, sampling_rate, duration, freq);
        break;
    }

    for (int i = 0; i < n_samples; i++) {
        if (y[0] > threshold) {
            y.erase(y.begin());
            y.push_back(1);
        }
        else {
            y.erase(y.begin());
            y.push_back(0);
        }
    }
}

void generate_wave() {
    matplot::plot(x, y);
}
void show_plot() {
    matplot::show();
}

void clear() {
    x.clear();
    y.clear();
    matplot::cla();
}


    namespace py = pybind11;
    
    



PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: scikit_build_example

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    m.def("cos_wave", &cos_wave, R"pbdoc(
kosinus
 )pbdoc");

    m.def("sin_wave", &sin_wave, R"pbdoc(
sinus
 )pbdoc");

    m.def("square_wave", &square_wave, R"pbdoc(
square
 )pbdoc");

    m.def("saw_wave", &saw_wave, R"pbdoc(
piloksztaltny
 )pbdoc");

    m.def("generate_wave", &generate_wave, R"pbdoc(
generacja fali
 )pbdoc");

    m.def("show_plot", &show_plot, R"pbdoc(
pokazanie wykresu
 )pbdoc");
    
    m.def("wave_threshold", &wave_threshold, R"pbdoc(
pokazanie wykresu
 )pbdoc");

    m.def("audio", &audio, R"pbdoc(
        test audiofile

    )pbdoc");

    m.def("process", &process, R"pbdoc(
        image filtration

    )pbdoc");

    m.def("function_wave", &function_wave, R"pbdoc(generowanie funkcji testowej)pbdoc");

    m.def("clear", &clear, R"pbdoc(czyszczenie)pbdoc");


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
