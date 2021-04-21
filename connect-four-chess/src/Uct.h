/*************************************************************************
    > File Name: Uct.h
    > Author: stout
    > Mail: 2817253603@qq.com 
    > Created Time: Mon 13 May 2019 02:14:11 AM CST
 ************************************************************************/

#pragma once
#include <vector>
#include <cstdio>
#include <ctime>
#include <cmath>

#include "Judge.h"

using namespace std;

#define TIME_LIMIT 0.5
#define NOTEND 2
#define RADIO 1.5

class State{
    public:
        // transfer in during construct
        int row = 0;
        int col = 0;
        int nx = -1;
        int ny = -1;
        int lx = -1;
        int ly = -1;
        vector<int> tp;
        vector<vector<int>> bd;

        // may be transfer in during construct
        int player = -1;
        State* father = NULL;

        // not transfer in but init during construct
        // expandedChild init:0  update:1->expanded -1->can't be expanded
        vector<int> epc;
        //child: init:NULL update:State*
        vector<State*> child;

        // update when function called
        int visitNum = 0;
        int profit = 0;

    public:
        State(int row_, int col_, int nx_, int ny_, int lx_, int ly_, vector<int> tp_, vector<vector<int>> bd_, int player_ = -1, State* father_ = NULL):
            row(row_),
            col(col_),
            nx(nx_),
            ny(ny_),
            lx(lx_),
            ly(ly_),
            tp(tp_),
            bd(bd_),
            player(player_),
            father(father_)
            {
                for(int i = 0;i < col;i ++){
                    epc.push_back(0);
                    child.push_back(NULL);
                }
            }
        void clear();
        void debug();
        State* hpc();
        State* mvc();
        State* expand();
        bool isEnd();
        bool isExpandable();
        bool haveChild(){
            for(int i = 0;i< col;i ++)
                if(child[i])
                    return true;
            return false;
        }
        ~State(){
            clear();
        }
};

bool State::isEnd(){
    if(lx == - 1 && ly == -1)
        return false;
    if(userWin(lx, ly, row, col, bd) || machineWin(lx, ly, row, col, bd))
        return true;
    if(isTie(col, tp))
        return true;
    return false;
}

void State::clear(){
    for(int i = 0;i < col;i ++)
        if(child[i])
            delete child[i];
}

bool State::isExpandable(){
    for(int i = 0;i < col;i ++){
        if(epc[i] == 0 && tp[i] > 0)
            return true;
    }
    return false;
}

// most visit child
State* State::mvc(){
    State* res = NULL;
    int maxVisitNum = -100;
    for(int i = 0;i < col;i ++){
        if(child[i]){
            if(child[i]->visitNum > maxVisitNum){
                maxVisitNum = child[i]->visitNum;
                res = child[i];
            }
        }
    }
    return res;
}

// highest profit child
State* State::hpc(){
    State* res = NULL;
    double maxProfitRadio = -1e100;
    int which = -1;
    for(int i = 0;i < col;i ++){
        if(this->child[i]){
            double p = double(this->player) * double(this->child[i]->profit);
            double vf = double(this->visitNum);
            double vc = double(this->child[i]->visitNum);
            double pr = p/vc + RADIO*sqrtl(2*log(vf)/vc);
            if(pr > maxProfitRadio){
                which = i;
                maxProfitRadio = pr;
                res = this->child[i];
            }
        }
    }
    // printf("hpc %d \n", which);
    if(res == NULL)
        printf("bug!!!!!\n");
    return res;
}

State* State::expand(){
    for(int i = 0;i < col;i ++){
        // i child have not been expand
        if(epc[i] == 0){
            if(tp[i] > 0){
                epc[i] = 1;

                // define new-parameter for child
                int newlx = -1;
                int newly = -1;
                vector<int> newtp = this->tp;
                vector<vector<int>> newbd = this->bd;

                // update new-parameter
                newtp[i] --;
                newlx = newtp[i]; newly = i;
                newbd[newlx][newly] = this->player;
                if(newtp[i] == nx && i == ny)
                    newtp[i] --;

                // new-child
                this->child[i] = new State(row, col, nx, ny, 
                        newlx, newly, newtp, newbd, -player, this);

                return this->child[i];
            }
            else{
                epc[i] = -1;
            }
        }
    }
    return NULL;
}

// state debug info output
void State::debug() {
	printf("current State:\n");
	if (lx == -1 && ly == -1) {
		printf("new board no one down the chess\n");
	}
	else {
		printf("player: %d  down chess at  %d  %d\n", -player, lx, ly);
	}
	if(player == 1)
		printf("next player:  A \n");
	else
		printf("next player:  B \n");
	
	printf("M %d N %d ", row, col);
	printf("top:  ");
	for (int i = 0; i < col; i++)
		printf("%d ", tp[i]);
	printf("\n");
	printf("board: \n");
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (bd[i][j] == 0) {
				if (i == nx && j == ny)
					printf("x ");
				else
					printf(". ");
			}
			if (bd[i][j] == 1) {
				if (i == lx && j == ly) {
					printf("\033[1;31;40mA \033[0m");
				}
				else
					printf("A ");
			}
			if (bd[i][j] == -1) {
				if (i == lx && j== ly) {
					printf("\033[1;31;40mB \033[0m");
				}
				else
					printf("B ");
			}
		}
		printf("\n");
	}
    printf("\n");
}


class Uct{
    public:
        int row = 0;
        int col = 0;
        int nx = -1;
        int ny = -1;
        int lx = -1;
        int ly = -1;
        State* root = NULL;
        
    public:
        Uct(int row_, int col_, int nx_, int ny_, int lx_, int ly_):
            row(row_),
            col(col_),
            nx(nx_),
            ny(ny_),
            lx(lx_),
            ly(ly_){}
        State* expandPolicy(State* node)const;
        int simulatePolicy(State* node);
        void playPolicy(State& node);
        int profitPolicy(State& node);
        void backupPolicy(State* node, int profit);
        State* searchPolicy(vector<int> tp, vector<vector<int>> bd);
        ~Uct(){
            delete root;
        }
};

// random drop one chess
void Uct::playPolicy(State& node){
    node.ly = rand() % node.col;
    while(node.tp[node.ly] == 0)
        node.ly = rand() % node.col;
    node.tp[node.ly] --;
    node.lx = node.tp[node.ly];
    node.bd[node.lx][node.ly]  = node.player;
    if(node.lx - 1 == node.nx && node.ly == node.ny)
        node.tp[node.ly] --;
    node.player = -node.player;
}

// calculate last down chess influence to the State
int Uct::profitPolicy(State& node){
    if(node.lx == -1 && node.ly == -1)
        return 0;
    // node.player == -1account for the user just operate is 1(user)
    if(node.player == -1 && userWin(node.lx, node.ly, node.row, node.col, node.bd)){
        return 1;
    }
    // node.player == 1 account for the user just operate is -1(machine)
    if(node.player == 1 && machineWin(node.lx, node.ly, node.row, node.col, node.bd)){
        return -1;
    }
    if(isTie(node.col, node.tp))
        return 0;
    return NOTEND;
}

// simulate one time to the expanded node
int Uct::simulatePolicy(State* node){
    //  copy a new-State to operate , avoid influence the origin node
    State tmp = *node;
    tmp.profit = profitPolicy(tmp);
    while(tmp.profit == NOTEND){
        // rand down one chess
        playPolicy(tmp);
        tmp.profit = profitPolicy(tmp);
    }
    return tmp.profit;
}

void Uct::backupPolicy(State* node, int profit){
    while(node){
        node->visitNum ++;
        node->profit += profit;
        node = node->father;
    }
}

State* Uct::expandPolicy(State* node)const{
    while(!node->isEnd()){
        if(node->isExpandable()){
            node = node->expand();
            break;
        }
        else{
            node = node->hpc();
        }
    }
    return node;
}

State* Uct::searchPolicy(vector<int> tp, vector<vector<int>> bd){
    root = new State(row, col, nx, ny, lx, ly, tp, bd);
    root->debug();
    double startTime = clock();
    int profit = 0;
    while(double(clock() - startTime) / CLOCKS_PER_SEC < TIME_LIMIT){
        for(int i = 0;i < 1000;i ++){
            State* node = expandPolicy(root);
            profit = simulatePolicy(node);
            backupPolicy(node, profit);
        }
    } 
    return root->mvc();
}
