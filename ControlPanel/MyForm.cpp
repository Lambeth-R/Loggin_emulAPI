#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;


int main(array<String^>^ arg) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	ControlPanel::MyForm form; //WinFormsTest - имя вашего проекта
	Application::Run(% form);
	return 0;
}
