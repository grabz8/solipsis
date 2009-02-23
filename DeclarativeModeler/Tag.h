/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Yoshimasa Tsuruoka (http://www-tsujii.is.s.u-tokyo.ac.jp/~tsuruoka/postagger/), updated by Sebastien LAIGRE

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef TAG_H_
#define TAG_H_


#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <list>
#include <sstream>
#include "maxent.h"
#include "common.h"

using namespace std;

static bool IGNORE_DISTINCTION_BETWEEN_NNP_AND_NN = false;

static const int NUM_TRAIN_SENTENCES = 99999999;

static const int NUM_TEST_SENTENCES = 99999999;

static std::vector< ME_Model > m_vme;

ME_Sample mesample(const vector<Token> &vt, int i, const string & prepos);
void viterbi(vector<Token> & vt, const ME_Model & me);
string postag(const string & s, const ME_Model & me);
string bidir_postag(const string & s, const std::vector<ME_Model> & vme /*, std::string& errMsg */);

//static void bidir_postagging(vector<Sentence> & vs, const multimap<string, string> & tagdic, const vector<ME_Model> & vme);
//static int bidir_train(const vector<Sentence> & vs, int para);

/*static*/ int train(const vector<Sentence> & vs, ME_Model & me)
{
  vector<ME_Sample> train_samples;

  cerr << "extracting features...";
  int n = 0;
  for (vector<Sentence>::const_iterator i = vs.begin(); i != vs.end(); i++) {
    const Sentence & s = *i;
    for (unsigned int j = 0; j < s.size(); j++) {
      string prepos = "BOS";
      if (j > 0) prepos = s[j-1].pos;
      train_samples.push_back(mesample(s, j, prepos));
    }
  }
  cerr << "done" << endl;

  me.set_heldout(1000, 0);
  me.train(train_samples, 0, 1000);
  return n;
}

/*static*/ void deterministic(vector<Token> & vt, const ME_Model & me)
{
  for (unsigned int i = 0; i < vt.size(); i++) {
    string prepos = "BOS";
    if (i > 0) prepos = vt[i-1].prd;
    //    string prepos2 = "BOS";
    //    if (i > 1) prepos2 = vt[i-2].prd;
    //    ME_Sample mes = mesample(vt, i, prepos, prepos2);
    ME_Sample mes = mesample(vt, i, prepos);
    //    vector<double> membp(me.num_classes());
    //    vt[i].prd = me.classify(mes, &membp);
    vector<double> membp = me.classify(mes);
    vt[i].prd = mes.label;
  }
}

/*static*/ void postagging(vector<Sentence> & vs, const ME_Model & me)
{
  cerr << "now tagging";
  int n = 0;
  for (vector<Sentence>::iterator i = vs.begin(); i != vs.end(); i++) {
    Sentence & s = *i;
    deterministic(s, me);
    if (n++ % 10 == 0) cerr << ".";
  }
  cerr << endl;
}

/*static*/ void evaluate(const vector<Sentence> & vs)
{
  int ncorrect = 0;
  int ntotal = 0;
  for (vector<Sentence>::const_iterator i = vs.begin(); i != vs.end(); i++) {
    const Sentence & s = *i;
    for (unsigned int j = 0; j < s.size(); j++) {
      ntotal++;
      string pos = s[j].pos;
      string prd = s[j].prd;
      if (IGNORE_DISTINCTION_BETWEEN_NNP_AND_NN) {      
        if (pos == "NNP") pos = "NN";
        if (pos == "NNPS") pos = "NNS";
        if (prd == "NNP") prd = "NN";
        if (prd == "NNPS") prd = "NNS";
      }
      if (pos == prd) ncorrect++;
      //      cout << s[j].str << "\t" << s[j].pos << "\t" << s[j].prd << endl;
    }
  }
  fprintf(stderr, "%d / %d = %f\n", ncorrect, ntotal, (double)ncorrect / ntotal);
}

/*static*/ void load_tag_dictionary(const string & filename, multimap<string, string> & tagdic)
{
  tagdic.clear();
  ifstream ifile(filename.c_str());
  if (!ifile) {
    cerr << "cannot open " << filename << endl;
    exit(1);
  }
  string line;
  while (getline(ifile, line)) {
    istringstream is(line.c_str());
    string str, tag;
    is >> str >> tag;
    tagdic.insert(pair<string, string>(str, tag));
  }
  ifile.close();
}

/*static*/ void make_tag_dictionary(const vector<Sentence> & vs)
{
  multimap<string, string> wt;
  for (vector<Sentence>::const_iterator i = vs.begin(); i != vs.end(); i++) {
    const Sentence & s = *i;
    for (unsigned int j = 0; j < s.size(); j++) {
      bool found = false;
      for (multimap<string, string>::const_iterator k = wt.lower_bound(s[j].str);
      k !=  wt.upper_bound(s[j].str); k++) {
        if (k->second ==  s[j].pos) { found = true; break; }
      }
      if (found) continue;
      wt.insert(pair<string, string>(s[j].str, s[j].pos));
    }
  }
  for (multimap<string, string>::const_iterator i = wt.begin(); i != wt.end(); i++) {
    cout << i->first << '\t' << i->second << endl;
  }

}

/*static*/ void read_pos(const string & filename, vector<Sentence> & vs, int num_sentences = -1)
{
  ifstream ifile(filename.c_str());

  string line;
  int n = 0;
  cerr << "reading " << filename;
  while (getline(ifile,line)) {
    istringstream is(line);
    string s;
    Sentence sentence;
    while (is >> s) {
      string::size_type i = s.find_last_of('/');
      string str = s.substr(0, i);
      string pos = s.substr(i+1);
      if (str == "-LRB-" && pos == "-LRB") { str = "("; pos = "("; }
      if (str == "-RRB-" && pos == "-RRB") { str = ")"; pos = ")"; }
      if (str == "-LSB-" && pos == "-LSB") { str = "["; pos = "["; }
      if (str == "-RSB-" && pos == "-RSB") { str = "]"; pos = "]"; }
      if (str == "-LCB-" && pos == "-LCB") { str = "{"; pos = "{"; }
      if (str == "-RCB-" && pos == "-RCB") { str = "}"; pos = "}"; }
      Token t(str, pos);
      sentence.push_back(t);
    }
    vs.push_back(sentence);
    if (vs.size() >= num_sentences) break;
    if (n++ % 10000 == 0) cerr << ".";
  }
  cerr << endl;

  ifile.close();
}

/*static*/ void loadVME()
{
	const unsigned int ME_Model_Size = 16;
	m_vme.clear();
	for( unsigned int i = 0; i < ME_Model_Size; i++ ) {
		char buf[1024];
		sprintf( buf, "./models/model.bidir.%d", i );
		ME_Model me;
		me.load_from_file( buf );
		m_vme.push_back( me );
	}
}

/*static*/ void tag( std::string & in,std::string & out /*, std::string& errMsg */ )
{
  std::string _text = in;

  //loadVME(); // already done at DeclarativeModeler creation

  // tag
  out = bidir_postag( _text, m_vme /*, errMsg*/ );
}

/*static*/ void extractTags(std::string & input, std::string & outFileName)
{
  // dÃ©claration d'une chaÃ®ne qui contiendra la ligne lue
  std::string texte = input;  

  std::string tmpString = texte;

  // recuperation des tags SEULS
  std::string tags;

  int indice = tmpString.find('/');
  while(indice!=-1)
    {
      int ind_space = tmpString.find(' ');

      tags = tags + tmpString.substr(indice+1, ind_space-indice);
      tmpString = tmpString.substr(ind_space+1);
      indice = tmpString.find('/');
      ind_space = tmpString.find(' ');
      if(ind_space==-1)
        {
          tags = tags + '.';
          break;
        }
    }

  FILE* outFile = NULL;
  outFile = fopen(outFileName.c_str(), "w");
  if (outFile != NULL)
    {
      // On l'écrit dans le fichier
      fprintf(outFile,tags.c_str());
      fclose(outFile);
    }
}

#endif /*TAG_H_*/