      std::ofstream out(path);
        out << "Chave inglesa,15,12.00\n";
        out << "Macaco,5,45.00\n";
	        out << "Porcas,50,0\n";
        out << "Pernos,40,0.20\n";
        out << "Pistoes,8,150.00\n";
        out << "Vela de ignicao,20,7.50\n";
        out << "Filtro de oleo,10,15.50\n";
        out << "Radiador,3,220.00\n";
        out << "Bateria,8,120.00\n";



        std::cout << "1) Listar stock\n";
        std::cout << "2) Registar venda\n";
        std::cout << "3) Adicionar peca\n";
        std::cout << "4) Alterar quantidade/preco\n";
        std::cout << "5) Guardar e sair\n";
        std::cout << "6) Sair sem guardar\n";
        std::cout << "Escolha uma opcao: ";
        int opt;
