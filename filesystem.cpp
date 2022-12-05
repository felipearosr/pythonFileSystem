#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <map>
#include <algorithm>
#include <threads.h>
#include <chrono>
using namespace std;
namespace fs = std::filesystem;

class Node                                      //Formato print? | Crear directorios en cualquier punto? | Opciones archivos?
{
  public:
    string thingy;
    string name;
    Node* root;
    vector<Node *> branch;

    Node(string route){
        root = NULL;
        thingy = route;
        name = route.substr(route.find_last_of("/")+1);
    }
    void grow_branch(Node* branch_to_grow){
        branch.push_back(branch_to_grow);
    }

    void print_tree(){
        cout << thingy << endl;
    }
    void print_branches(){
        for(int i = 0; i<branch.size(); i++){
            cout << branch[i]->thingy << endl;
        }
    }
};



void mkdir(Node* workdir, string dir_name){
    bool insert = true;
    for(int i=0; i<workdir->branch.size();i++){
        if(workdir->branch[i]->name == dir_name){
            insert = false;
            break;
        }
    }
    if(insert){
        string aux = workdir->thingy+"/"+dir_name;
        Node* dir_name = new Node(aux);  
        fs::create_directory(aux);
    }
    cout << "niggas in paris" << endl;

    cout << "niggas in prague" << endl;
}

void pwd(Node* tree_node){
    tree_node->print_tree();
}

void find_branches(Node* main){
    string c_wd;
    string path = main->thingy;
    for (const auto & entry : fs::directory_iterator(path)){
        c_wd =  entry.path();
        Node* node_to_insert = new Node(c_wd);
        node_to_insert->root = main;
        main->grow_branch(node_to_insert);
    }
}

void update_registry(Node* main){
    bool insert;
    string c_wd;
    string path = main->thingy;
    for (const auto & entry : fs::directory_iterator(path)){
        insert = true;
        c_wd =  entry.path();
        for(int i = 0; i<main->branch.size(); i++){
            if(main->branch[i]->thingy == c_wd){
                // cout << "thingy: " << main->branch[i]->thingy << endl;
                // cout << "sthingy: " << c_wd << endl;
                // cout << "-----------------------------" << endl;
                insert = false;
                break;
            }
        }
        if(insert == true){
            Node* node_to_insert = new Node(c_wd);
            node_to_insert->root = main;
            main->grow_branch(node_to_insert);
            // cout << "inserted: " << c_wd << endl;
        }
    }
    for(int i=0; i<main->branch.size();i++){
        if(fs::is_directory(main->branch[i]->thingy)){
            update_registry(main->branch[i]);
        }
    }
}

Node* cd(Node* workdir, string target_dir){
    Node* change_context = workdir;
    if(target_dir == "-" && workdir->root != NULL){
        workdir = workdir->root;
        return workdir;
    }
    else{
        for(int i = 0; i < workdir->branch.size(); i++){                            //Optimizar con main->name? check
            // int pos = workdir->branch[i]->thingy.find_last_of("/");
            // string sub_c_wd = workdir->branch[i]->thingy.substr(pos+1);
            //cout << "stuff:" << sub_c_wd << endl;
            if(workdir->branch[i]->name == target_dir && fs::is_directory(workdir->branch[i]->thingy)){
                change_context = workdir->branch[i];
                //workdir = workdir->branch[i];
                break;
            }
        }
        //workdir->print_tree();
        return change_context;
    }
}

void ls(Node* main){
    Node* ls = main;
    string c_wd;
    string path = main->thingy;

    // for (const auto & entry : fs::directory_iterator(path)){            //Método 1: Iteración sobre sistema utilizando filesystem / path
    //     c_wd =  entry.path();
    //     int pos = c_wd.find_last_of("/");
    //     string sub_c_wd = c_wd.substr(pos+1);
    //     cout << sub_c_wd << endl;
    // }
    for(int i=0; i < ls->branch.size(); i++){                           //Método 2: Iteración sobre vector->name
        cout << ls->branch[i]->name << endl;
    }
}

void recursive_deletion(Node* workdir){
    if(fs::is_directory(workdir->thingy) && workdir->branch.empty() != true){
        cout << "nodo: " << workdir->name << " vector size: " << workdir->branch.size() << endl;
        for(int i=0;i<workdir->branch.size();i++){
            Node* re_iterate = workdir->branch[i];
            workdir->branch.erase(workdir->branch.begin() + i);
            recursive_deletion(re_iterate);
        }
        recursive_deletion(workdir);
    }
    else{
        fs::remove(workdir->thingy);
        delete workdir;
    }
}

void rm(Node* workdir, string target){
    for(int i = 0; i<workdir->branch.size(); i++){
        if(workdir->branch[i]->name == target){
            if(fs::is_directory(workdir->branch[i]->thingy)){
                Node* to_remove = workdir->branch[i];
                workdir->branch.erase(workdir->branch.begin() + i);
                recursive_deletion(to_remove);
            }
            else{
                fs::remove(workdir->branch[i]->thingy);
                delete workdir->branch[i];
                workdir->branch.erase(workdir->branch.begin() + i);
            }
        }
    }
}

void mkfile(Node* workdir, string file){
    bool insert = true;
    for(int i=0; i<workdir->branch.size();i++){
        if(workdir->branch[i]->name == file){
            insert = false;
            break;
        }
    }
    if(insert){
        string aux = workdir->thingy+"/"+file;
        Node* file = new Node(aux);
        ofstream Myfile(aux);
        Myfile.close();
    }
}

Node* clip(Node* workdir, Node* clipboard, string target){
    for(int i=0; i<workdir->branch.size();i++){
        if(workdir->branch[i]->name == target){
            cout << workdir->branch[i]->name << endl;
            cout << target << endl;
            clipboard = workdir->branch[i];
            break;
        }
    }
    return clipboard;
}

void pclip(Node* workdir, Node* clipboard){
    bool insert = true;
    for(int i=0; i<workdir->branch.size();i++){

        if(workdir->branch[i]->name == clipboard->name){
            insert = false;
        }
    }
    if(insert){
        fs::copy(clipboard->thingy, workdir->thingy+"/"+clipboard->name);
        workdir->grow_branch(clipboard);
    }
}

void mv(Node* workdir, string to_move, string target){              //This many IF's suck [to be fixed].
    bool insert = false;
    bool is_full_path = false;
    // if(target.at(0) == '/'){
    //     if(fs::exists(target)){
    //         cout << "BIG PROBLEM!!!1!!!!111" << endl;
    //     }
    //     else{
    //         fs::copy(to_move, target);
    //         rm(workdir, to_move);
    //     }
    // }
    // else{
        for(int i=0; i<workdir->branch.size(); i++){                        
            if(workdir->branch[i]->name == to_move){
                cout << "true?" << endl;
                insert = true;
                break;
            }
            if(workdir->branch[i]->thingy == to_move){
                cout << "true?" << endl;
                is_full_path = true;
                insert = true;
                break;
            }
        }
        if(insert){                                                         //PROBLEMA PATHS!! -> MV
            if(target.at(0) == '/'){
                if(fs::exists(target)){
                    cout << "BIG PROBLEM!!!1!!!!111" << endl;
                }
                else{

                    fs::copy(to_move, target, std::filesystem::copy_options::recursive);
                    rm(workdir, to_move);
                }  
            }
            else{
                if(fs::exists(workdir->thingy+"/"+target)){
                    cout << "BIG PROBLEM!!!1!!!!111" << endl;
                }
                else{
                    fs::copy(to_move, workdir->thingy+"/"+target);
                    rm(workdir, to_move);
                }                  
            }
        }
    // }


    // if(insert){
    //     if(fs::exists(target)){
    //         cout << "big problem" << endl;
    //     }
    //     else{
    //         fs::copy(to_move, target);
    //         rm(workdir, to_move);
    //     }  
    // }
}

void rnm(Node* workdir, string n_name){

}

void recursivetree(Node* tree, int counter){ 
    if(tree->branch.empty()){
        for(int i=0; i<counter; i++){
            cout<<"-";
        }
        cout << ">" << tree->name << endl;
    }
    else{

        for(int i=0; i<counter; i++){
            cout<<"-";
        }
        counter++;
        cout << ">" << tree->name << endl;
        for(int i = 0; i<tree->branch.size();i++){
            recursivetree(tree->branch[i], counter);
            pthread_join;
        }
    }
}

void showmetreeplz(Node* tree){
    int ct = 0;
    recursivetree(tree, ct);
}

int main(int argc, char *argv[]){
    //SYSCALLS-begin

    map<string, int> syscalls;
    syscalls["mkdir"] = 1;
    syscalls["pwd"] = 2;
    syscalls["cd"] = 3;
    syscalls["ls"] = 4;
    syscalls["rm"] = 5;
    syscalls["mkfile"] = 6;
    syscalls["mv"] = 7;
    syscalls["cp"] = 8;
    syscalls["exit"] = 9;
    syscalls["showmetreeplz"] = 10;
    syscalls["clip"] = 11;
    syscalls["pclip"] = 12;
    syscalls["show_c"] = 13;

    //SYSCALLS-end
    cout << "STARTING PERUVIAN FILESYSTEM MK1..." << endl;
    cout << "Utilice show_c para ver una lista de comandos" << endl;
    string c_wd = fs::current_path();
    string input_val;
    string extra_input;
    string sys_val;

    Node* init = new Node(c_wd);                             //Nodo inicialización/cambio de directorio.                            
    Node* workdir = init;                                    //Nodo working directory.
    Node* clipboard = new Node("");
    update_registry(workdir);

    while(true){
        cout << ">";
        cin >> sys_val;
        switch(syscalls[sys_val]){
            case 1:
                //cout << "Nombre nuevo directorio:" << endl;
                cin >> input_val;
                mkdir(workdir, input_val);
                update_registry(workdir);
            break;
            case 2:
                pwd(workdir);
            break;
            case 3:
                //cout << "target directory" << endl;
                cin >> input_val;
                init = cd(workdir, input_val);
                workdir = init;
                update_registry(workdir);
            break;
            case 4:
                ls(workdir);
            break;
            case 5:
                //cout << "Archivo/directorio a eliminar" << endl;
                cin >> input_val;
                rm(workdir, input_val);
            break;
            case 6:
                //cout << "nombre nuevo archivo" << endl;
                cin >> input_val;
                mkfile(workdir, input_val);
                update_registry(workdir);
            break;
            case 7:
                //cout << "archivo/carpeta a mover" << endl;
                cin >> input_val;
                //cout << "target directory" << endl;
                cin >> extra_input;
                mv(workdir, input_val, extra_input);
                update_registry(workdir);
                //workdir->print_branches();
            break;
            case 8:
            break;
            case 9:
                exit(EXIT_FAILURE);
            break;
            case 10:
                showmetreeplz(workdir);
            break;
            case 11:
                //cout << "Directorio/Archivo a copiar" << endl;
                cin >> input_val;
                clipboard = clip(workdir, clipboard, input_val);
            break;
            case 12:
                pclip(workdir, clipboard);
            break;
            case 13:
                cout << "----------------------------------------------------" << endl;
                for (auto const &pair: syscalls) {
                    std::cout << pair.first << endl;
                }
                cout << "----------------------------------------------------" << endl;
            break;
        }
    }
}




