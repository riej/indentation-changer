#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <glob.h>
#include <list>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

int g_OriginalIndentationSize = 0;

int g_NewIndentationSize = 0;
char g_NewIndentationCharacter = ' ';

// https://gist.github.com/lucianmachado/9a26d5745497ffe5d054
inline list<string> glob(const string& pat){
  using namespace std;
  glob_t glob_result;
  glob(pat.c_str(), GLOB_TILDE, NULL, &glob_result);
  list<string> ret;
  for(unsigned int i = 0;i < glob_result.gl_pathc; ++i) {
    ret.push_back(string(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
  return ret;
}

void ChangeIndentation(const string & filename) {
  ifstream t(filename);
  string buffer;

  t.seekg(0, ios::end);
  buffer.reserve(t.tellg());
  t.seekg(0, ios::beg);

  buffer.assign(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());

  string result = "";
  bool newline = true;
  int spacesCount = 0;
  for (unsigned int i = 0; i < buffer.length(); i++) {
    char c = buffer[i];
    if (newline) {
      if (c == ' ' || c == '\t') {
        spacesCount++;
      } else {
        int newIndent = (int)floor((float)spacesCount * g_NewIndentationSize / g_OriginalIndentationSize);
        for (; newIndent; newIndent--) {
          result += g_NewIndentationCharacter;
        }

        newline = false;
        spacesCount = 0;
      }
    }

    if (!newline) {
      if (c == '\n' || c == '\r') {
        newline = true;
      }
      result += c;
    }
  }

  t.close();

  ofstream t2(filename);
  t2 << result;
  t2.close();
}

void ShowHelp() {
  cout << "indentation-changer --from <from-count> --to <to-count> [--tabs] <filename> [<filename 2>]\n";
  cout << "  --from <from-count>   - how much spaces or tabs used for indentation now\n";
  cout << "  --to <to-count>       - how much spaces needed\n";
  cout << "  --tabs                - use tabs instead of spaces (in result)\n";
  cout << "  <filename>            - file name (wildcards * and ? supported)\n";
}

int main(int argc, char ** argv) {
  list<string> filenames;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) {
      ShowHelp();
      return 0;
    } else if (strcmp(argv[i], "--from") == 0 && i + 1 < argc) {
      g_OriginalIndentationSize = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--to") == 0 && i + 1 < argc) {
      g_NewIndentationSize = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--tabs") == 0 || strcmp(argv[i], "--tab") == 0) {
      g_NewIndentationCharacter = '\t';
      if (g_NewIndentationSize == 0) {
        g_NewIndentationSize = 1;
      }
    } else {
      filenames.merge(glob(argv[i]));
    }
  }

  if (g_OriginalIndentationSize == 0 || g_NewIndentationSize == 0 || filenames.size() == 0) {
    ShowHelp();
    return 1;
  }

  for (std::list<string>::iterator it = filenames.begin(); it != filenames.end(); ++it) {
    ChangeIndentation(*it);
  }

  return 0;
}
