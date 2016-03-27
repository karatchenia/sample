//Copies the string (such as a password) from the stdin to the clipboard.

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

void report_error_and_exit(const string& error_message, const errno_t err = 0)
{
  const auto error_code = err ? err : GetLastError();
  cout << error_message << " Error code: " << error_code << ".\n";
  exit(0);
}

template<typename T>
inline void check(const T& isOk, const string& error_message)
{
  if (!isOk)
  {
    report_error_and_exit(error_message);
  }
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

  check(OpenClipboard(nullptr), "Error opening the clipboard.");
  check(EmptyClipboard(), "Error emptying the clipboard.");

  const auto buffer_size = (word.size() + 1) * sizeof(char);
  auto global_buffer = GlobalAlloc(GMEM_DDESHARE, buffer_size);
  check(global_buffer, "Error allocating the buffer.");

  auto buffer = static_cast<char*>(GlobalLock(global_buffer));
  check(buffer, "Error locking the buffer.");

  const auto err = strcpy_s(buffer, buffer_size, word.c_str());
  if (err)
  {
    report_error_and_exit("Error copying the string to the buffer.", err);
  }
  
  check(GlobalUnlock(global_buffer), "Error unlocking the buffer.");
  check(SetClipboardData(CF_TEXT, global_buffer), "Error setting the clipboard data.");
  check(CloseClipboard(), "Error closing the clipboard.");

  cout << word.size() << " symbol(s) copied to the clipboard.\n";
  return 0;
}
