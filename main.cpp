#include <iostream>
#include <vector>
#include "histogram.h"
#include "svg.h"
#include <curl/curl.h>
#include <sstream>
#include <string>
#include <windows.h>

using namespace std;

struct Input {
    vector<double> numbers;
    size_t bin_count;
    size_t number_count;
};

vector<double>
input_numbers(istream& in, size_t count) {
    vector<double> result(count);
    for (size_t i = 0; i < count; i++) {
        in >> result[i];
    }
    return result;
}

Input
read_input(istream& in, bool prompt) {
    Input data;
    size_t number_count;
    in >> number_count;
    if (prompt)
        cerr << "Enter number count: ";
    if (prompt)
        cerr << "Enter numbers: ";
    data.numbers = input_numbers(in, number_count);

    if (prompt)
        cerr << "Enter column count: ";
    in >> data.bin_count;
    data.number_count = number_count;
    return data;
}

vector<size_t>
make_histogram(Input data){
    vector<size_t>result (data.bin_count);
    double min, max;
    find_minmax(data.numbers, min, max);
    for (double number : data.numbers) {
        size_t bin = (size_t)((number - min) / (max - min) * data.bin_count);
        if (bin == data.bin_count) {
            bin--;
        }
        result[bin]++;
    }
    return result;
}

void
show_histogram_text(const auto bins){
    const size_t SCREEN_WIDTH = 80;
    const size_t MAX_ASTERISK = SCREEN_WIDTH - 4 - 1;

    size_t max_count = 0;
    for (size_t count : bins) {
        if (count > max_count) {
            max_count = count;
        }
    }
    const bool scaling_needed = max_count > MAX_ASTERISK;

    for (size_t bin : bins) {
        if (bin < 100) {
            cout << ' ';
        }
        if (bin < 10) {
            cout << ' ';
        }
        cout << bin << "|";

        size_t height = bin;
        if (scaling_needed) {
            const double scaling_factor = (double)MAX_ASTERISK / max_count;
            height = (size_t)(bin * scaling_factor);
        }

        for (size_t i = 0; i < height; i++) {
            cout << '*';
        }
        cout << '\n';
    }
}

size_t
write_data(void* items, size_t item_size, size_t item_count, void* ctx) {
    size_t data_size = item_size * item_count;
    stringstream* buffer = reinterpret_cast<stringstream*>(ctx);
    buffer->write(reinterpret_cast<const char*>(items), data_size);
    return data_size;
}

Input
download(const string& address) {
    stringstream buffer;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        if (res!=CURLE_OK){
            cerr << curl_easy_strerror(res) << endl;
            exit(1);
        }
        curl_easy_cleanup(curl);
    }
    return read_input(buffer, false);
}

int
main(int argc, char* argv[]) {

    Input input;
    string format;
    if (argc > 1) {

        int URL_position = 0;
        string option = "-format";
        char* URL = "http://";
        for (int i = 0; i < argc; i++){
            if (option == argv[i]){
                format = argv[i+1];
            }
        }
        for (int i = 0; i < argc; i++){
            if (strstr(argv[i],URL) !=NULL)
                URL_position = i;
        }
        if (URL_position == 0){
            cerr << "Incorrect input. URL not founded.";
                return 0;
        }
        input = download(argv[URL_position]);
    }
    else {
        input = read_input(cin, true);
    }

    const auto bins = make_histogram(input);
    if (format == "svg")
        show_histogram_text(bins);
    if (format == "text")
        show_histogram_svg(bins);
    if (format == "")
        cerr << "Please input format";
}
