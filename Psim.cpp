/* On my honor, I have neither given nor received unauthorized aid on this assignment */


#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <unistd.h>

using namespace std;

string pprint(deque<string> Q){
    string temp = "";
        for(auto it = Q.begin();it!=Q.end();it++){
        temp+=*it;
        temp+=",";
    }
    if(temp.size()>1){
    temp.pop_back();}
    return temp;
}




// Instruction Memory it can have up to 16 Instructions
typedef struct INM
{
    bool flag = 0;
    deque <string> B;
}INM;

//Instruction Buffer
typedef struct INB
{
    bool flag = 0;
    deque <string> B;
}INB;

//Arithmetic Instruction Buffer
typedef struct AIB
{
    bool flag = 0;
    deque <string> B;
}AIB;

//Partial Result Buffer
typedef struct PRB
{
    bool flag = 0;
    deque <string> B;
}PRB;

//Result Buffer
typedef struct REB
{
    bool flag = 0;
    deque <string> B;
}REB;

//Address Buffer
typedef struct ADB
{
    bool flag = 0;
    deque <string> B;
}ADB;

//Storage Instruction Buffer
typedef struct SIB
{
    bool flag = 0;
    deque <string> B;
}SIB;


//Converts the instuction into vector
vector<string> parse(string Exp){
    vector<string> T;
    auto a = strtok(const_cast<char  *>(Exp.c_str()),"<>,");
    while(a!=NULL){
        T.emplace_back(a);
        a = strtok(NULL, "<>,");
    }
    return T;
}
//Convert the vector into an instruction
string serialize(vector<string> In){
    string I = "<";
    int i = 0;
    for(i = 0;i<In.size()-1;i++){
        I = I + In[i] + ",";
    }
    I = I + In[i];
    I = I + ">";
    return I;
}



vector<string> ftov(string filename){ // file to a vector conversion
    vector<string> v;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line))
            v.push_back(line);
         file.close();
    }
    return v;
}




class ARM // ARM Processor
{
    public:
        INM INM_v;
        INB INB_v;
        AIB AIB_v;
        PRB PRB_v;
        REB REB_v;
        ADB ADB_v;
        SIB SIB_v;
        unordered_map<string,int> RGF; // Register File
        map<int,int> DAM; // Data Memory

    void loadInstructions(string filepath){
        auto v = ftov(filepath);
        INM_v.flag = 1;
        for(int i=0;i<v.size();i++){
            INM_v.B.push_back(v[i]);
        }
    }
    void loadMemory(string filepath){
        auto v = ftov(filepath);
        for(int i = 0;i<v.size();i++){
            auto temp = parse(v[i]);
            DAM[stoi(temp[0])] = stoi(temp[1]);
        }
    }
    void loadRegisters(string filepath){
        auto v = ftov(filepath);
        for(int i = 0;i<v.size();i++){
            auto temp = parse(v[i]);
            RGF[temp[0]] = stoi(temp[1]);
        }
    }
    void decodeRead(){ // Decode and Read
        if(INM_v.flag){
            string t = INM_v.B.front();
            INM_v.B.pop_front();
            vector<string> vt = parse(t);
            if(vt[0]!="ST"){
                vt[2] = to_string(RGF[vt[2]]);
                vt[3] = to_string(RGF[vt[3]]);
            }
            else{
                vt[2] = to_string(RGF[vt[2]]);
            }
            t = serialize(vt);
            INB_v.B.push_back(t);
            if(INB_v.flag==0){
                INB_v.flag = 1;
            }
            if(INM_v.B.size()==0){
                INM_v.flag = 0;
            }
        }

    }
    void issue1(){
        if(INB_v.flag){
            auto t = INB_v.B.front();
            vector<string> vt = parse(t);
            if(vt[0]!="ST"){
                INB_v.B.pop_front();
                AIB_v.B.push_back(t);
                if(AIB_v.flag==0){
                    AIB_v.flag = 1;
                }
                if(INB_v.B.size()==0){
                    INB_v.flag = 0;
                }
            }
        }
    }

    void issue2(){
        if(INB_v.flag){
            auto t = INB_v.B.front();
            vector<string> vt = parse(t);
            if(vt[0]=="ST"){
                INB_v.B.pop_front();
                SIB_v.B.push_back(t);
                if(SIB_v.flag==0){
                    SIB_v.flag = 1;
                }
                if(INB_v.B.size()==0){
                    INB_v.flag = 0;
                }
            }
        }
    }

void addr(){ // Format should be <R,1>
        if(SIB_v.flag){
            string t = SIB_v.B.front();
            vector<string> vt = parse(t);
            SIB_v.B.pop_front();
            if(SIB_v.B.size()==0){
                SIB_v.flag = 0;
            }
            int sum = stoi(vt[2])+stoi(vt[3]);
            string a = to_string(sum);
            string b = vt[1];
            vector<string> temp;
            temp.push_back(b);
            temp.push_back(a);
            string t2 = serialize(temp);
            ADB_v.B.push_back(t2);
            if(ADB_v.flag==0){
                ADB_v.flag = 1;
            }
        }
    }

void store(){
    if(ADB_v.flag){
        auto t = ADB_v.B.front();
        vector<string> vt = parse(t);
        ADB_v.B.pop_front();
        if(ADB_v.B.size()==0){
            ADB_v.flag = 0;
        }
    DAM[stoi(vt[1])] = RGF[vt[0]];
    }
}

    void asu(){
        if(AIB_v.flag){
            auto t = AIB_v.B.front();
            vector<string> vt = parse(t);
            vector<string> temp;
            if(vt[0]=="ADD"){
                temp = {vt[1],to_string(stoi(vt[2])+stoi(vt[3]))};
                AIB_v.B.pop_front();
                if(AIB_v.B.size()==0){
                    AIB_v.flag = 0;
                }
            }
            else if(vt[0]=="SUB"){
                temp = {vt[1],to_string(stoi(vt[2])-stoi(vt[3]))};
                AIB_v.B.pop_front();
                if(AIB_v.B.size()==0){
                    AIB_v.flag = 0;
                }

            }
            string tt = serialize(temp);
            REB_v.B.push_back(tt);
            if(REB_v.flag==0){
                REB_v.flag = 1;
            }
        }
    }


    void mul1(){
        if(AIB_v.flag){
            auto t = AIB_v.B.front();
            vector<string> vt = parse(t);
            vector<string> temp;
            if(vt[0]=="MUL"){
                AIB_v.B.pop_front();
                if(AIB_v.B.size()==0){
                    AIB_v.flag = 0;
                }
                PRB_v.B.push_back(t);
                if(PRB_v.flag==0){
                    PRB_v.flag = 1;
                }
            }
        }
    }

    void mul2(){
        if(PRB_v.flag){
            auto t = PRB_v.B.front();
            vector<string> vt = parse(t);
            PRB_v.B.pop_front();
            if(PRB_v.B.size()==0){
                PRB_v.flag = 0;
            }
            vector<string> tt = {vt[1],to_string(stoi(vt[2])*stoi(vt[3]))};
            REB_v.B.emplace_back(serialize(tt));
            if(REB_v.flag==0){
                REB_v.flag = 1;
            }
        }
    }
    void write(){
        if(REB_v.flag){
            auto t = REB_v.B.front();
            vector<string> vt = parse(t);
            REB_v.B.pop_front();
            if(REB_v.B.size()==0){
                REB_v.flag = 0;
            }
            RGF[vt[0]] = stoi(vt[1]);
        }
    }

    string memoryState(){
        string temp = "";
        for(auto it=DAM.begin();it!=DAM.end();it++){
            temp+="<"+to_string(it->first)+","+to_string(it->second)+">"+",";
        }
        temp.pop_back();
        return temp;
    }

    string registerState(){
        string temp = "";
        for(int i=0;i<=15;i++){
            string key = "R"+to_string(i);
            if(RGF.find(key)!=RGF.end()){
                temp+="<R"+to_string(i)+","+to_string(RGF[key])+">"+",";
            }
        }
        temp.pop_back();
        return temp;

    }
    void compute(){

        store();
        addr();
        issue2();
        write();
        mul2();
        mul1();
        asu();
        issue1();
        decodeRead();


    }
    void computeAll(){
        int count = 0;
        cout<<"STEP "<<count<<":"<<endl;
        snapshot();
        while(INM_v.flag or INB_v.flag or AIB_v.flag or SIB_v.flag or PRB_v.flag or ADB_v.flag or REB_v.flag){
            count++;
            compute();
            cout<<"STEP "<<count<<":"<<endl;
            snapshot();
        }
    }

    void computeAllFile(){
        FILE * pFile;
        pFile = fopen ("simulation.txt","w");
        int count = 0;
            string t1 = pprint(INM_v.B);
            string t2 = pprint(INB_v.B);
            string t3 = pprint(AIB_v.B);
            string t4 = pprint(SIB_v.B);
            string t5 = pprint(PRB_v.B);
            string t6 = pprint(ADB_v.B);
            string t7 = pprint(REB_v.B);
            string t8 = registerState();
            string t9 = memoryState();
        
            fprintf(pFile,"STEP %d:\n",count);
            fprintf(pFile,"INM:%s \n",t2.c_str());
            fprintf(pFile,"INB:%s \n",t2.c_str());
            fprintf(pFile,"AIB:%s \n",t3.c_str());
            fprintf(pFile,"SIB:%s \n",t4.c_str());
            fprintf(pFile,"PRB:%s \n",t5.c_str());
            fprintf(pFile,"ADB:%s \n",t6.c_str());
            fprintf(pFile,"REB:%s \n",t7.c_str());
            fprintf(pFile,"RGF:%s \n",t8.c_str());
            fprintf(pFile,"DAM:%s \n",t9.c_str());
            fprintf(pFile,"\n");
        while(INM_v.flag or INB_v.flag or AIB_v.flag or SIB_v.flag or PRB_v.flag or ADB_v.flag or REB_v.flag){
            count++;
            compute();
            t1 = pprint(INM_v.B);
            t2 = pprint(INB_v.B);
            t3 = pprint(AIB_v.B);
            t4 = pprint(SIB_v.B);
            t5 = pprint(PRB_v.B);
            t6 = pprint(ADB_v.B);
            t7 = pprint(REB_v.B);
            t8 = registerState();
            t9 = memoryState();
            fprintf(pFile,"STEP %d:\n",count);
            fprintf(pFile,"INM:%s \n",t1.c_str());
            fprintf(pFile,"INB:%s \n",t2.c_str());
            fprintf(pFile,"AIB:%s \n",t3.c_str());
            fprintf(pFile,"SIB:%s \n",t4.c_str());
            fprintf(pFile,"PRB:%s \n",t5.c_str());
            fprintf(pFile,"ADB:%s \n",t6.c_str());
            fprintf(pFile,"REB:%s \n",t7.c_str());
            fprintf(pFile,"RGF:%s \n",t8.c_str());
            fprintf(pFile,"DAM:%s \n",t9.c_str());
            fprintf(pFile,"\n");
        }
         fclose(pFile);
    }

    // void snapshot2(){
    // string t1 = pprint(INM_v.B);
    // printf("%s \n",t1.c_str());
    // cout<<"INM:"<<pprint(INM_v.B)<<endl;
    // cout<<"INB:"<<pprint(INB_v.B)<<endl;
    // cout<<"AIB:"<<pprint(AIB_v.B)<<endl;
    // cout<<"SIB:"<<pprint(SIB_v.B)<<endl;
    // cout<<"PRB:"<<pprint(PRB_v.B)<<endl;
    // cout<<"ADB:"<<pprint(ADB_v.B)<<endl;
    // cout<<"REB:"<<pprint(REB_v.B)<<endl;
    // cout<<"RGF:"<<registerState()<<endl;
    // cout<<"DAM:"<<memoryState()<<endl;
    // cout<<endl;

    // }

    void snapshot(){
    string t1 = pprint(INM_v.B);
    string t2 = pprint(INB_v.B);
    string t3 = pprint(AIB_v.B);
    string t4 = pprint(SIB_v.B);
    string t5 = pprint(PRB_v.B);
    string t6 = pprint(ADB_v.B);
    string t7 = pprint(REB_v.B);
    string t8 = registerState();
    string t9 = memoryState();
    printf("INM:%s \n",t1.c_str());
    printf("INB:%s \n",t2.c_str());
    printf("AIB:%s \n",t3.c_str());
    printf("SIB:%s \n",t4.c_str());
    printf("PRB:%s \n",t5.c_str());
    printf("ADB:%s \n",t6.c_str());
    printf("REB:%s \n",t7.c_str());
    printf("RGF:%s \n",t8.c_str());
    printf("DAM:%s \n",t9.c_str());
    printf("\n");

    }




};


int main(){
    ARM p1;
    p1.loadInstructions("instructions.txt");
    p1.loadMemory("datamemory.txt");
    p1.loadRegisters("registers.txt");
    //p1.computeAll();
    p1.computeAllFile();

    return 0;
}
