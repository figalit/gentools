////////////////////////////
//   convert_blast.cpp    //
//     by Kerry Hall      //
//   created 10/10/06     //
// last modified 10/16/06 //
//                        //
////////////////////////////

#include "convert_blast.h"
#include <cstdlib>
#include <string>

//#define HIGH 50000
//#define LOW 20000

using namespace std;

struct read{
  string name;
  int begin;
  int end;

  int third;
  int fourth;
  int fifth;
  string sixth;


  string chr;
};

struct cloneinfo{
  vector<struct read> forwards;
  vector<struct read> reverses;
};

typedef map<string, cloneinfo> mapdata;
typedef mapdata::value_type mappair;
ofstream fout;
ofstream singleout;
int HIGH, LOW;
int MAXLEN;


void func1(const mappair &val){
  int totalwin;
  int flag;
  int clonestart;
  int cloneend;
  int i;

  if (val.second.forwards.size() == 0){
    for (i=0;i<val.second.reverses.size();i++){
      singleout << val.second.reverses.at(i).name << "\t";
      singleout << val.second.reverses.at(i).begin << "\t"; // chromosome begin
      singleout << val.second.reverses.at(i).end << "\t"; // chromosome end
      singleout << val.second.reverses.at(i).third << "\t"; // query begin
      singleout << val.second.reverses.at(i).fourth << "\t"; // query end
      singleout << val.second.reverses.at(i).fifth << "\t"; // # windows
      singleout << val.second.reverses.at(i).sixth << "\t"; // orientation
      singleout << val.second.reverses.at(i).chr << "\n"; // chromosome name       
    }
  }
  else if (val.second.reverses.size() == 0){
    for (i=0;i<val.second.forwards.size();i++){
      singleout << val.second.forwards.at(i).name << "\t";
      singleout << val.second.forwards.at(i).begin << "\t"; // chromosome begin
      singleout << val.second.forwards.at(i).end << "\t"; // chromosome end
      singleout << val.second.forwards.at(i).third << "\t"; // query begin
      singleout << val.second.forwards.at(i).fourth << "\t"; // query end
      singleout << val.second.forwards.at(i).fifth << "\t"; // # windows
      singleout << val.second.forwards.at(i).sixth << "\t"; // orientation
      singleout << val.second.forwards.at(i).chr << "\n"; // chromosome name       
    }
  }
  for(i =0;i<val.second.forwards.size();i++){
    //look for a corresponding reverse
    for(int j=0;j<val.second.reverses.size();j++){


      int distance = abs(val.second.forwards.at(i).begin - val.second.reverses.at(j).begin);
      int rdist;
      if (val.second.forwards.at(i).begin < val.second.reverses.at(j).begin){
	clonestart = val.second.forwards.at(i).begin;
	cloneend = val.second.reverses.at(j).end;
      }
      else{	
	clonestart = val.second.reverses.at(j).begin;
	cloneend = val.second.forwards.at(i).end;
      }


      if (val.second.forwards.at(i).name.compare("000308_1229_3619.FORWARD.1") == 0 && val.second.reverses.at(j).name.compare("000308_1229_3619.REVERSE.1") == 0){
	  //val.second.reverses.at(j).begin==23987810){
	//cout << "distance: " << distance << "\n" ; 
	cout << val.second.forwards.at(i).begin << "\t" << val.second.reverses.at(j).begin << "\t" << distance << "\n";
      }

      //if (val.second.forwards.at(i).chr==val.second.reverses.at(j).chr) {
      if (!val.second.forwards.at(i).chr.compare(val.second.reverses.at(j).chr)) {

	if (distance > MAXLEN)
	  continue;


	fout<<val.first<<"\t"<<val.second.forwards.at(i).name<<"\t"<<val.second.forwards.at(i).chr<<"\t"<<val.second.forwards.at(i).begin<<"\t"<<val.second.forwards.at(i).end<<"\t"<<val.second.forwards.at(i).third<<"\t"<<val.second.forwards.at(i).fourth<<"\t"<<val.second.forwards.at(i).fifth<<"\t"<<val.second.forwards.at(i).sixth<<"\t";
	fout<<val.second.reverses.at(j).name<<"\t"<<val.second.reverses.at(j).chr<<"\t"<<val.second.reverses.at(j).begin<<"\t"<<val.second.reverses.at(j).end<<"\t"<<val.second.reverses.at(j).third<<"\t"<<val.second.reverses.at(j).fourth<<"\t"<<val.second.reverses.at(j).fifth<<"\t"<<val.second.reverses.at(j).sixth<<"\t";

	
	totalwin = (val.second.forwards.at(i).fifth)+(val.second.reverses.at(j).fifth);
	fout<<distance<<"\t";
	
	fout<<totalwin<<"\t";
	
	flag = 1;

    	if (!val.second.forwards.at(i).sixth.compare("F") && !val.second.reverses.at(j).sixth.compare("R")){
	    	rdist = val.second.forwards.at(i).begin - val.second.reverses.at(j).begin; 
		if (rdist > 0){
		  fout<<"wrongorient\t";
		  flag = 0;
		}
	}

	else if (!val.second.forwards.at(i).sixth.compare("R") && !val.second.reverses.at(j).sixth.compare("F")){
	    	rdist = val.second.reverses.at(j).begin - val.second.forwards.at(i).begin; 
	    	if (rdist > 0){
		  fout<<"wrongorient\t";
		  flag = 0;
		}
	}

	if (flag) {
	  if (distance<=HIGH && distance>=LOW){
	    if (val.second.forwards.at(i).sixth.compare(val.second.reverses.at(j).sixth) == 0)
	      fout<<"inversion\t";
	    else
	      fout<<"concordant\t";
	    
	  }
	  else if (distance > HIGH){
	    if (val.second.forwards.at(i).sixth.compare(val.second.reverses.at(j).sixth) == 0)
	      fout<<"delinv\t";
	    else
	      fout<<"deletion\t";
	  }
	  else{
	    if (val.second.forwards.at(i).sixth.compare(val.second.reverses.at(j).sixth) == 0)
	      fout<<"insinv\t";
	    else
	      fout<<"insertion\t";
	  }
	}
	fout<<val.second.reverses.at(j).chr<<"\t";   // true for non-transchromosomal clones
	fout<<clonestart<<"\t";
	fout<<cloneend<<"\t";
	fout<<"\n";


	j = val.second.reverses.size();
      }
      
      else{ /*transchromosomal*/
	
	fout<<val.first<<"\t"<<val.second.forwards.at(i).name<<"\t"<<val.second.forwards.at(i).chr<<"\t"<<val.second.forwards.at(i).begin<<"\t"<<val.second.forwards.at(i).end<<"\t"<<val.second.forwards.at(i).third<<"\t"<<val.second.forwards.at(i).fourth<<"\t"<<val.second.forwards.at(i).fifth<<"\t"<<val.second.forwards.at(i).sixth<<"\t";
	fout<<val.second.reverses.at(j).name<<"\t"<<val.second.reverses.at(j).chr<<"\t"<<val.second.reverses.at(j).begin<<"\t"<<val.second.reverses.at(j).end<<"\t"<<val.second.reverses.at(j).third<<"\t"<<val.second.reverses.at(j).fourth<<"\t"<<val.second.reverses.at(j).fifth<<"\t"<<val.second.reverses.at(j).sixth<<"\t";

	totalwin = (val.second.forwards.at(i).fifth)+(val.second.reverses.at(j).fifth);
	fout<<"NA\t"; // distance is NA for transchromosomals
 	
	fout<<totalwin<<"\t";
	
	fout<<"transchromosomal"<<"\t";
	
	fout<<"NA\t"; // clonechr is NA for transchromosomals
	fout<<"NA\t"; // clonestart is NA for transchromosomals
	fout<<"NA\t"; // cloneend is NA for transchromosomals
	fout<<"\n";


	j = val.second.reverses.size();


      }

    }
  }





}





convert_blast::convert_blast(){
	f_input = "";
	f_output = "converted.out";
	myHIGH = 48000;
	myLOW = 32000;
	myMAXLEN = 1000000;
}

void convert_blast::compute(){
  ifstream in(f_input.c_str());
  string line;
  map<string, cloneinfo> data;
  int fpos, rpos;
  HIGH = myHIGH;
  LOW = myLOW;
  MAXLEN = myMAXLEN;

  while (!in.eof()){
    getline(in, line);
    stringstream * ss;
    ss = new stringstream(line);
    string token;
    vector<string> tokens;
    while(*ss>>token){
      tokens.push_back(token);
    } 
    delete ss;

    if (tokens.size()>0){
      //convert the first token to a clone name;

      


      string clonename = tokens.at(0);
      
      fpos = clonename.find(".FORWARD",0);
      rpos = clonename.find(".REVERSE",0);


      if (fpos != -1){
	clonename.erase(fpos);
	struct read foo;
	foo.name = tokens.at(0);
	foo.begin = atoi(tokens.at(1).c_str());
	foo.end = atoi(tokens.at(2).c_str());
	foo.chr = tokens.at(7);
	foo.third=atoi(tokens.at(3).c_str());
	foo.fourth=atoi(tokens.at(4).c_str());
	foo.fifth=atoi(tokens.at(5).c_str());
	foo.sixth=tokens.at(6).substr(0,1);

	data[clonename].forwards.push_back(foo);
	continue;
      }
      else if (rpos != -1){
	clonename.erase(rpos);
	struct read foo;
	foo.name = tokens.at(0);
	foo.begin = atoi(tokens.at(1).c_str());
	foo.end = atoi(tokens.at(2).c_str());
	foo.chr = tokens.at(7);
	foo.third=atoi(tokens.at(3).c_str());
	foo.fourth=atoi(tokens.at(4).c_str());
	foo.fifth=atoi(tokens.at(5).c_str());
	foo.sixth=tokens.at(6).substr(0,1);

	data[clonename].reverses.push_back(foo);
	continue;
      }

      //find the first instance of an F or the first instnace of an R
      fpos = clonename.find("F",0);
      rpos = clonename.find("R",0);

      if (rpos == -1) rpos = 999;
      if (fpos == -1) fpos = 999;
      
      string temp;
      if (fpos<rpos){ //the name says its a forward
       
	temp = clonename.substr(fpos+1, clonename.length()-1);
      	clonename.erase(fpos,fpos+1);
	clonename+=temp;
      	//clonename.erase(clonename.length()-1);
	struct read foo;
	foo.name = tokens.at(0);
	foo.begin = atoi(tokens.at(1).c_str());
	foo.end = atoi(tokens.at(2).c_str());
	foo.chr = tokens.at(7);
	foo.third=atoi(tokens.at(3).c_str());
	foo.fourth=atoi(tokens.at(4).c_str());
	foo.fifth=atoi(tokens.at(5).c_str());
	foo.sixth=tokens.at(6).substr(0,1);

	data[clonename].forwards.push_back(foo);
      } else if (rpos<fpos){ //the name says its a reverse
	
	temp = clonename.substr(rpos+1, clonename.length()-1);
      	clonename.erase(rpos,rpos+1);
	clonename+=temp;
      	//clonename.erase(clonename.length()-1);
	struct read foo;
	foo.name = tokens.at(0);
	foo.begin = atoi(tokens.at(1).c_str());
	foo.end = atoi(tokens.at(2).c_str());
	foo.chr = tokens.at(7);
	foo.third=atoi(tokens.at(3).c_str());
	foo.fourth=atoi(tokens.at(4).c_str());
	foo.fifth=atoi(tokens.at(5).c_str());
	foo.sixth=tokens.at(6).substr(0,1);

        data[clonename].reverses.push_back(foo);
      }
      
    }

  }
  
  in.close();

  //////
  //////
  //////map is finished, now analyize it.



  fout.open(f_output.c_str());
  f_output.append(".single");
  singleout.open(f_output.c_str());
  for_each(data.begin(), data.end(), func1);
  fout.close();








}
