//Copies the string (such as a password) from the stdin to the clipboard.

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

void report_error_and_exit(const string& decription, const unsigned long long err = 0)
{
  const auto error_code = err ? err : GetLastError();
  cout << decription << " Error code: " << error_code << ".\n";
  exit(0);
}

int main()
{
  string word;
  cin >> word;
  if (0 == word.size())
  {
    cout << "The input word must be passed in the standard input.\nUsage: ClipBoardSetter.exe < SecretFile.txt\n";
    return 0;
  }

  if (!OpenClipboard(nullptr))
  {
    report_error_and_exit("Error opening the clipboard.");
  }

  if (!EmptyClipboard())
  {
    report_error_and_exit("Error emptying the clipboard.");
  }

  const auto buffer_size = (word.size() + 1) * sizeof(char);
  auto global_buffer = GlobalAlloc(GMEM_DDESHARE, buffer_size);
  if (!global_buffer)
  {
    report_error_and_exit("Error allocating the buffer.");
  }

  auto buffer = static_cast<char*>(GlobalLock(global_buffer));
  if (!buffer)
  {
    report_error_and_exit("Error locking the buffer.");
  }

  const auto err = strcpy_s(buffer, buffer_size, word.c_str());
  if (err)
  {
    report_error_and_exit("Error copying the string to the buffer.", err);
  }
  
  if (!GlobalUnlock(global_buffer))
  {
    report_error_and_exit("Error unlocking the buffer.");
  }
  
  if (!SetClipboardData(CF_TEXT, global_buffer))
  {
    report_error_and_exit("Error setting the clipboard data.");
  }

  if (!CloseClipboard())
  {
    report_error_and_exit("Error closing the clipboard.");
  }

  return 0;
}
