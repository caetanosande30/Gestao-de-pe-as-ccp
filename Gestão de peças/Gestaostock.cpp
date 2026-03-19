#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <random>
#include <limits>
#include <cctype>
#include <filesystem>

struct Item {
    std::string name;
    int quantity;
    double price;
};

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

static std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
    return out;
}

std::vector<Item> loadStock(const std::string& path) {
    std::vector<Item> stock;
    std::ifstream in(path);
    if (!in) {

       //Adição de produtoa
        std::ofstream out(path);
        out << "Chave inglesa,15,12.00\n";
        out << "Macaco,5,45.00\n";
        out << "Porcas,50,0.10\n";
        out << "Pernos,40,0.20\n";
        out << "Pistoes,8,150.00\n";
        out << "Vela de ignicao,20,7.50\n";
        out << "Filtro de oleo,10,15.50\n";
        out << "Radiador,3,220.00\n";
        out << "Bateria,8,120.00\n";
        out.close();
        in.open(path);
    }
    
    if (in && in.peek() != std::ifstream::traits_type::eof()) {
        // load from file
        std::string line;
        while (std::getline(in, line)) {
            if (trim(line).empty()) continue;
            std::istringstream ss(line);
            std::string name, qstr, pstr;
            // Expect CSV: name,quantity,price
            if (!std::getline(ss, name, ',')) continue;
            if (!std::getline(ss, qstr, ',')) continue;
            if (!std::getline(ss, pstr)) continue;
            Item it;
            it.name = trim(name);
            try {
                it.quantity = std::stoi(trim(qstr));
            }
            catch (...) { it.quantity = 0; }
            try {
                it.price = std::stod(trim(pstr));
            }
            catch (...) { it.price = 0.0; }
            stock.push_back(it);
        }
        return stock;
    }

    // If file does not exist or is empty, create defaults with random stock (1..10)
    const std::vector<std::pair<std::string, double>> defaults = {
        {"Chave inglesa", 12.00},
        {"Macaco", 45.00},
        {"Porcas", 0.10},
        {"Pernos", 0.20},
        {"Pistoes", 150.00},
        {"Vela de ignicao", 7.50},
        {"Filtro de oleo", 15.50},
        {"Radiador", 220.00},
        {"Bateria", 120.00}
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 10);
    for (const auto& p : defaults) {
        Item it;
        it.name = p.first;
        it.price = p.second;
        it.quantity = dist(gen);
        stock.push_back(it);
    }

    // save initial stock
    std::ofstream out(path, std::ios::trunc);
    for (const auto& it : stock) {
        out << it.name << "," << it.quantity << "," << std::fixed << std::setprecision(2) << it.price << "\n";
    }
    out.close();

    return stock;
}

void saveStock(const std::string& path, const std::vector<Item>& stock) {
    std::ofstream out(path, std::ios::trunc);
    for (const auto& it : stock) {
        out << it.name << "," << it.quantity << "," << std::fixed << std::setprecision(2) << it.price << "\n";
    }
}

void listStock(const std::vector<Item>& stock) {
    std::cout << "\nID  Nome                      Qt.  Preco\n";
    std::cout << "--------------------------------------------\n";
    for (size_t i = 0; i < stock.size(); ++i) {
        const auto& it = stock[i];
        std::cout << std::setw(2) << i + 1 << "  "
            << std::left << std::setw(24) << it.name << " "
            << std::right << std::setw(4) << it.quantity << "  "
            << std::fixed << std::setprecision(2) << it.price;
        if (it.quantity <= 5) std::cout << "   <-- ALERTA: 5 ou menos unidades";
        std::cout << "\n";
    }
}

int main() {
    const std::string path = "stock.txt";
    auto stock = loadStock(path);
    try {
        std::cout << "Ficheiro de stock: " << std::filesystem::absolute(path) << "\n";
    }
    catch (...) {
        std::cout << "Ficheiro de stock: " << path << "\n";
    }

    while (true) {
        std::cout << "\nGestao de Stock - Auto-Pecas\n";
        std::cout << "1) Listar stock\n";
        std::cout << "2) Registar venda\n";
        std::cout << "3) Adicionar peca\n";
        std::cout << "4) Alterar quantidade/preco\n";
        std::cout << "5) Guardar e sair\n";
        std::cout << "6) Sair sem guardar\n";
        std::cout << "Escolha uma opcao: ";
        int opt;
        if (!(std::cin >> opt)) {
            std::cin.clear();
            std::string dummy; std::getline(std::cin, dummy);
            std::cout << "Entrada invalida. Tente novamente.\n";
            continue;
        }
        if (opt == 1) {
            listStock(stock);
        }
        else if (opt == 2) {
            listStock(stock);
            std::cout << "\nDigite o ID da peca a vender: ";
            int id;
            if (!(std::cin >> id)) {
                std::cin.clear(); std::string dummy; std::getline(std::cin, dummy);
                std::cout << "Entrada invalida.\n"; continue;
            }
            if (id < 1 || id >(int)stock.size()) {
                std::cout << "ID invalido.\n"; continue;
            }
            Item& it = stock[id - 1];
            std::cout << "Quantidade disponivel: " << it.quantity << "\n";
            std::cout << "Quantidade a vender: ";
            int q;
            if (!(std::cin >> q)) {
                std::cin.clear(); std::string dummy; std::getline(std::cin, dummy);
                std::cout << "Entrada invalida.\n"; continue;
            }
            if (q <= 0) { std::cout << "Quantidade deve ser positiva.\n"; continue; }
            if (q > it.quantity) {
                std::cout << "Stock insuficiente. Venda nao registada.\n";
                continue;
            }
            it.quantity -= q;
            double total = q * it.price;
            std::cout << "Venda registada. Total: " << std::fixed << std::setprecision(2) << total << "\n";
            if (it.quantity <= 5) std::cout << "ALERTA: " << it.name << " tem 5 ou menos unidades (" << it.quantity << ").\n";
            // save change so next run resumes where left
            saveStock(path, stock);
        }
        else if (opt == 3) {
            // Adicionar peca ou repor unidades se ja existir
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\nNome da peca: ";
            std::string name;
            std::getline(std::cin, name);
            name = trim(name);
            if (name.empty()) { std::cout << "Nome vazio. Operacao cancelada.\n"; continue; }

            // procurar por nome existente (case-insensitive)
            std::string nameLower = toLower(name);
            int foundIndex = -1;
            for (size_t i = 0; i < stock.size(); ++i) {
                if (toLower(trim(stock[i].name)) == nameLower) { foundIndex = (int)i; break; }
            }

            if (foundIndex != -1) {
                Item& existing = stock[foundIndex];
                std::cout << "Peca ja existe: '" << existing.name << "' - Quantidade atual: " << existing.quantity << "\n";
                std::cout << "Deseja adicionar unidades a esta peca? (s/n): ";
                std::string ans;
                std::getline(std::cin, ans);
                if (ans.empty()) { std::cout << "Operacao cancelada.\n"; continue; }
                char c = std::tolower(ans[0]);
                if (c == 's' || c == 'y') {
                    std::cout << "Quantas unidades adicionar: ";
                    int addQ;
                    if (!(std::cin >> addQ) || addQ <= 0) {
                        std::cin.clear(); std::string dummy; std::getline(std::cin, dummy);
                        std::cout << "Quantidade invalida. Operacao cancelada.\n"; continue;
                    }
                    existing.quantity += addQ;
                    std::cout << "Unidades adicionadas. Nova quantidade: " << existing.quantity << "\n";
                    // save change
                    saveStock(path, stock);
                    // consumir o restante da linha
                    std::string dummy; std::getline(std::cin, dummy);
                }
                else {
                    std::cout << "Operacao cancelada.\n";
                }
            }
            else {
                std::cout << "Quantidade inicial: ";
                int q;
                if (!(std::cin >> q) || q < 0) {
                    std::cin.clear(); std::string dummy; std::getline(std::cin, dummy);
                    std::cout << "Quantidade invalida. Operacao cancelada.\n"; continue;
                }
                std::cout << "Preco unitario: ";
                double p;
                if (!(std::cin >> p) || p < 0.0) {
                    std::cin.clear(); std::string dummy; std::getline(std::cin, dummy);
                    std::cout << "Preco invalido. Operacao cancelada.\n"; continue;
                }
                Item newItem;
                newItem.name = name;
                newItem.quantity = q;
                newItem.price = p;
                stock.push_back(newItem);
                std::cout << "Peca adicionada com sucesso.\n";
                // save change
                saveStock(path, stock);
            }
        }
        else if (opt == 4) {
            // Alterar quantidade ou preco de um produto
            listStock(stock);
            std::cout << "\nDigite o ID da peca a alterar: ";
            int id;
            if (!(std::cin >> id)) { std::cin.clear(); std::string dummy; std::getline(std::cin, dummy); std::cout << "Entrada invalida.\n"; continue; }
            if (id < 1 || id >(int)stock.size()) { std::cout << "ID invalido.\n"; continue; }
            Item& it = stock[id - 1];
            std::cout << "Peca: " << it.name << " - Quantidade: " << it.quantity << " - Preco: " << std::fixed << std::setprecision(2) << it.price << "\n";
            std::cout << "1) Alterar quantidade\n";
            std::cout << "2) Alterar preco\n";
            std::cout << "Escolha: ";
            int sub;
            if (!(std::cin >> sub)) { std::cin.clear(); std::string dummy; std::getline(std::cin, dummy); std::cout << "Entrada invalida.\n"; continue; }
            if (sub == 1) {
                std::cout << "Nova quantidade: ";
                int nq;
                if (!(std::cin >> nq) || nq < 0) { std::cin.clear(); std::string dummy; std::getline(std::cin, dummy); std::cout << "Quantidade invalida.\n"; continue; }
                it.quantity = nq;
                std::cout << "Quantidade atualizada.\n";
                saveStock(path, stock);
            }
            else if (sub == 2) {
                std::cout << "Novo preco: ";
                double np;
                if (!(std::cin >> np) || np < 0.0) { std::cin.clear(); std::string dummy; std::getline(std::cin, dummy); std::cout << "Preco invalido.\n"; continue; }
                it.price = np;
                std::cout << "Preco atualizado.\n";
                saveStock(path, stock);
            }
            else {
                std::cout << "Opcao invalida.\n";
            }
        }
        else if (opt == 5) {
            saveStock(path, stock);
            std::cout << "Stock guardado em '" << path << "'. Saindo.\n";
            break;
        }
        else if (opt == 6) {
            std::cout << "Saindo sem guardar.\n";
            break;
        }
        else {
            std::cout << "Opcao desconhecida.\n";
        }
    }

    return 0;
}
