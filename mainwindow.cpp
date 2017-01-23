#include "mainwindow.h"
#include "ui_mainwindow.h"

void sleep(int ms)						    //wait ms miliiseconds
{
    QTime t;							    //create time
    t.start();							    //start created time
    while(t.elapsed()<ms)
	QApplication::processEvents();				    //wait while time elapsed liss than need
}

QPixmap MainWindow::opacity(QPixmap pix, int alpha)		    //return the transparent copy of received pixmap
{
    QImage *image=new QImage(pix.width(),pix.height(),QImage::Format_ARGB32);//create mask
    image->fill(qRgb(alpha,alpha,alpha));
    pix.setAlphaChannel(QPixmap::fromImage(*image));		    //apply mask
    delete image;						    //delete mask
    return pix;							    //return result
}

bool MainWindow::parameter_exist(QString parameter_name)            //check is parameter exist in system's parameter file
{
    bool found=false;						    //create variable
    if(QFile::exists(file_parameters))				    //check is file exists
    {
	QFile paramF(file_parameters);				    //открываем файл для чтения
	bool ok=paramF.open(QIODevice::ReadOnly);
	if(ok==true)
	{
	    QDataStream param;					    //create stream
	    param.setDevice(&paramF);
	    QString name;					    //create temp variables
	    QString data;
	    while(!paramF.atEnd())				    //read parameters from file
	    {
		param>>name>>data;
		if(name==parameter_name)			    //if found
		    found=true;					    //set that parameter found
	    }
	}
	else
	{
	    QMessageBox::information(this, "Ошибка", "Возникла ошибка во время открытия файла параметров "+file_parameters+" для чтения. Далее программа может работать некорректно.");
	}
    }
    return found;						    //return result
}

QString MainWindow::parameter_get(QString parameter_name)           //get parameter data from system's parameter file
{
    QString Rdata="NULL";
    if(QFile::exists(file_parameters))				    //check is file exists
    {
	QFile paramF(file_parameters);				    //открываем файл для чтения
	bool ok=paramF.open(QIODevice::ReadOnly);
	if(ok==true)
	{
	    QDataStream param;					    //create stream
	    param.setDevice(&paramF);
	    QString name;
	    QString data;
	    while(!paramF.atEnd())				    //read parameters from file
	    {
		param>>name>>data;
		if(name==parameter_name)			    //if found
		{
		    Rdata=data;					    //write parameter's data to variable
		    break;
		}
	    }
	}
	else
	{
	    QMessageBox::information(this, "Ошибка", "Возникла ошибка во время открытия файла параметров "+file_parameters+" для чтения. Далее программа может работать некорректно.");
	}
    }
    return Rdata;						    //return result
}

void MainWindow::parameter_set(QString parameter_name, QString parameter_info)//set or add parameter is system's parameter file
{
    QString names[parameters_max_num];				    //create temp vars
    QString datas[parameters_max_num];
    if(QFile::exists(file_parameters))				    //check is file exists
    {
	QFile paramF(file_parameters);				    //open file to read and get all parameters
	bool ok=paramF.open(QIODevice::ReadOnly);
	if(ok==true)						    //if file opened correctly
	{
	    QDataStream param;
	    param.setDevice(&paramF);
	    int cur=0;						    //identificator used to calcilate parameters
	    bool saved=false;					    //true if parameter was found when copyind all parameters
	    while(!paramF.atEnd())					    //read parameters to variables
	    {
		param>>names[cur]>>datas[cur];
		names[cur+1]="NULL";				    //to stop when copying back
		if(names[cur]==parameter_name)			    //if it need parameter - save
		{
		    datas[cur]=parameter_info;
		    saved=true;
		}
		cur++;
	    }
	    if(saved==false)					    //if needed parameter not found-save in the end
	    {
		names[cur]=parameter_name;
		names[cur+1]="NULL";
		datas[cur]=parameter_info;
	    }
	    paramF.close();					    //close file after reading
	}
	else							    //if file don't opened correctly
	{
	    QMessageBox::critical(this, "Ошибка!", "Не удалось открыть для чтения файл параметров "+file_parameters+" в процессе записи параметра "+parameter_name);
	}
    }
    else							    //if parameters file don't exist
    {
	names[0]=parameter_name;				    //write parameter in the begining
        names[1]="NULL";
        datas[0]=parameter_info;
    }
    QFile paramF(file_parameters);				    //open file to write all parameters
    bool ok=paramF.open(QIODevice::WriteOnly);
    if(ok==true)						    //if file opened correctly
    {
	QDataStream param;					    //create ctream
	param.setDevice(&paramF);
	for(int i=0;names[i]!="NULL";i++)			    //write all parameters
	    param<<names[i]<<datas[i];
	paramF.close();						    //close file after writing
    }
    else							    //if file don't opened correctly
    {
	QMessageBox::critical(this, "Ошибка!", "Не удалось открыть для записи файл параметров "+file_parameters+" в процессе записи параметра "+parameter_name);
    }
}

void MainWindow::tooltip_set(QWidget* object,bool exist, QString text)//add tooltip to tooltips database
{
    for(int i=0;i<objects_num;i++)				    //for each object check
    {
	if(exist==true)						    //if need to add or replace command
	{
	    if(tooltips[i].exist==false || tooltips[i].object==object)//check if it is the needed command of last command
	    {
		tooltips[i].exist=true;				    //now it exist
		tooltips[i].object=object;			    //set our object
		tooltips[i].text=text;				    //set our text
		break;						    //go out from the cycle
	    }
	}
	if(exist==false)					    //if need to remove tooltip
	{
	    if(tooltips[i].object==object)			    //if it is needed object
	    {
		int j=i;					    //make moving objects after it object
		for(;j<objects_num-2 && tooltips[j+1].exist==true;j++)
		    tooltips[j]=tooltips[j+1];
		tooltips[j].exist=false;			    //and mark last object as not existing
		break;						    //go out from the cycle
	    }
	}
    }
}

int MainWindow::getObjNum(QLabel*label)				    //get number of object from the database by his adress
{
    int res=-1;							    //returns -1 if object not found
    for(int i=0;i<objects_num;i++)				    //for every object
    {
	if(par[i].obj==label)					    //if object found
	    res=i;						    //save it number
    }
    return res;							    //and return
}

void MainWindow::addCommand(QLabel* label,int aim, int step)	    //add command to animation(aims) database
{
    if(label==black_label)					    //if it is black label it need to be on the top
	black_label->raise();					    //bring to front
    for(int i=0;i<max_commands_num;i++)				    //for each command
    {
	if(commands[i].exist==false || commands[i].object==label)   //if it needed command or empty space
	{
	    commands[i].step=step;				    //set data there
	    commands[i].aim=aim;
	    commands[i].exist=true;
	    commands[i].object=label;
	    break;
	}
    }
    if(label==black_label && aim<10)				    //if need to hide black label we can to continue animation
	animation->start(timer_rate);
}

void MainWindow::setColor(QWidget* widget, QString normal, QString hover, QString pressed)//sets styleSheet of received object according to received colors
{
    widget->setStyleSheet("*{color:#"+normal+"} *:hover{color:#"+hover+"} *:pressed{color:#"+pressed+"} *:disabled{color:#777777}");
}

void MainWindow::setFont(QWidget* widget, QString name, int size, bool bold, bool italic, bool underlined)//sets the font of the received object according to received parameters
{
    QFont font;							    //create new font
    font.setFamily(name);					    //set it parameters
    font.setPointSize(size);
    font.setBold(bold);
    font.setItalic(italic);
    font.setUnderline(underlined);
    widget->setFont(font);					    //apply it
}

void MainWindow::checkboxes_recheck()				    //only recheck checkboxes on list page
{
    for(int i=0;i<checkboxes_num;i++)				    //for every checkbox on the page
    {
	if(pages[page_cur][i]>0)				    //if prog
	    chbox[i].setChecked(programs[pages[page_cur][i]-1].checked);
	if(pages[page_cur][i]<0)				    //if category name
	    chbox[i].setChecked(category_checked(-(pages[page_cur][i]+1)));//decide check or not
    }
}

void MainWindow::onThemeBox_changed(int num)			    //when select the theme
{
    addCommand(black_label,black_max,black_max/2);		    //dark window
    sleep(70);							    //wait while darking finishes
    apply_theme(num);						    //apply theme
    addCommand(black_label,0,black_off_speed);			    //undark window
}

bool MainWindow::category_checked(int category)			    //decide if need to check category
{
    bool checked=true;
    for(int i=0;i<programs_num;i++)				    //for all programs
	if(programs[i].category==category)
	    if(programs[i].checked==false)			    //if even one progam is not checked
		checked=false;					    //group is not checked
    return checked;						    //return result
}

void MainWindow::pages_clip()					    //animate different thing by quichly tramsparemt main area
{
    int main_area_obj_num=getObjNum(ui->main_area);                 //get number of main area in the database
    if(main_area_obj_num!=-1)					    //if object found
    {
	par[main_area_obj_num].opac=par[main_area_obj_num].opac_normal+((par[main_area_obj_num].opac_hovered-par[main_area_obj_num].opac_normal)/2);//set current opacity to less value
	addCommand(ui->main_area,par[main_area_obj_num].opac_hovered,30);//and add command to restore transparency
    }
}

void MainWindow::installing_button_1_clicked()			    //whem clicked button on instlalling page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    int file = 0;
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs[installing_selected[installing_cur]].files[file];
    if(QFile(path).exists())					    //if ifle exists
    {
	QUrl url;
	url.clear();
	url=QUrl(path);
	QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::installing_button_2_clicked()			    //whem clicked button on instlalling page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    int file = 1;
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs[installing_selected[installing_cur]].files[file];
    if(QFile(path).exists())					    //if ifle exists
    {
	QUrl url;
	url.clear();
	url=QUrl(path);
	QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::installing_button_3_clicked()			    //whem clicked button on instlalling page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    int file = 2;
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs[installing_selected[installing_cur]].files[file];
    if(QFile(path).exists())					    //if ifle exists
    {
	QUrl url;
	url.clear();
	url=QUrl(path);
	QDesktopServices::openUrl(url);				    //open generated url
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::installing_button_4_clicked()			    //whem clicked button on instlalling page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    int file = 3;
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs[installing_selected[installing_cur]].files[file];
    if(QFile(path).exists())					    //if ifle exists
    {
	QUrl url;
	url.clear();
	url=QUrl(path);
	QDesktopServices::openUrl(url);				    //open generated url
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::installing_button_5_clicked()			    //whem clicked button on instlalling page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    int file = 4;
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs[installing_selected[installing_cur]].files[file];
    if(QFile(path).exists())					    //if ifle exists
    {
	QUrl url;
	url.clear();
	url=QUrl(path);
	QDesktopServices::openUrl(url);				    //open generated url
    }
    else
    {
	QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
    }
}

void MainWindow::installing_button_open_clicked()                   //open program folder
{
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\";
    if(QFile(path).exists())					    //if ifle exists
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);				    //open generated url
    }
    else
    {
        QMessageBox::information(this,"Ошибка","Путь "+path+" не найден.");
    }
}

void MainWindow::installing_button_screenshot_clicked()		    //whem clicked button on instlalling page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs_file_screenshot;
    if(QFile(path).exists())					    //if ifle exists
    {
	QUrl url;
	url.clear();
	url=QUrl(path);
	QDesktopServices::openUrl(url);				    //open generated url
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::on_button_1_clicked()				    //run when click first button
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                    //play sound
    addCommand(black_label,black_max,black_max/2);		    //add connand to dark window while processing
    sleep(70);
    if(state_current==1)					    //if active is list page need to change it to program page
    {
	for(int i=0;i<checkboxes_num;i++)			    //save current checks
	    if(pages[page_cur][i]>0)
		programs[pages[page_cur][i]-1].checked=chbox[i].isChecked();
	bool empty=true;					    //проверим выбрана ли хоть одна программа
	for(int i=0;i<programs_num;i++)
	    if(programs[i].checked==true)
		empty=false;
	if(empty==false)					    //если хоть что-то да выбрано, можно продолжать
	{
            installing_selected_num=0;				    //null selected num variable
	    for(int i=0;i<programs_num;i++)                             //filter all programs
	    {
		if(programs[i].checked==true)			    //if program checked
		{
                    if(programs[i].description_ru=="file")//if need to get description from file
                    {
                        QString desc_file=programs_folder+categories[programs[i].category]+"\\"+programs[i].name+"\\"+programs_file_description;
                        if(QFile::exists(desc_file))		    //get current description
                        {
                            QFile vFile(desc_file);		    //open description file for reading
                            bool ok=vFile.open(QIODevice::ReadOnly);
                            if(ok==true)			    //if file opened correctly
                            {
                                QTextStream v(&vFile);		    //create ctream
                                QString desk_ru="";             //desk ru/desk ua/desk en
                                QString desk_ua="";
                                QString desk_en="";
                                QChar ch;
                                while(!v.atEnd())        		    //get description by char...
                                {
                                    v>>ch;
                                    if(ch=='/')
                                        break;
                                    desk_ru.append(ch);
                                    if(v.atEnd())
                                        break;
                                }
                                while(!v.atEnd())        		    //get description by char...
                                {
                                    v>>ch;
                                    if(ch=='/')
                                        break;
                                    desk_ua.append(ch);
                                    if(v.atEnd())
                                        break;
                                }
                                while(!v.atEnd())        		    //get description by char...
                                {
                                    v>>ch;
                                    if(ch=='/')
                                        break;
                                    desk_en.append(ch);
                                    if(v.atEnd())
                                        break;
                                }
                                vFile.close();			    //close file for reading
                                programs[i].description_ru=desk_ru;
                                if(desk_ua=="")
                                    programs[i].description_ua=desk_ru;
                                else
                                    programs[i].description_ua=desk_ua;
                                if(desk_en=="")
                                    programs[i].description_en=desk_ru;
                                else
                                    programs[i].description_en=desk_en;
                            }
                            else				    //if file don't opened correctly
                            {
                               programs[i].description_ru="Не удалось открыть файл с описанием";
                               programs[i].description_ua="Не вдалося відкрити файл з описом";
                               programs[i].description_en="Could not open a file with a description";
                            }
                        }
                        else					    //if file don't exist
                        {
                            programs[i].description_ru="Файл с описанием не найден";
                            programs[i].description_ua="Файл з описом не знайдено";
                            programs[i].description_en="Description file not found";
                        }
                    }
		    installing_selected[installing_selected_num]=i; //add current checked program checked list
		    installing_selected_num++;			    //and increment counter
		}
	    }
            int r=installing_selected_num;                                 //sort found variable
            for(int i=0;i<installing_selected_num;i++)
            {
                for(int j=1;j<r;j++)
                {
                    if(programs[installing_selected[j-1]].name>programs[installing_selected[j]].name)
                    {
                        int tmp=installing_selected[j-1];
                        installing_selected[j-1]=installing_selected[j];
                        installing_selected[j]=tmp;
                    }
                }
                r--;
            }
	    state_current=2;                                        //set current state to programs page
	    installing_cur=0;					    //jump to first page
	    state_update(state_current);			    //update state.
	}
	else
        {
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка!","Ни одной программы не выбрано. Мы не можем продолжить.");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка!","Жодної програми не обрано. Ми не можемо продовжити");
            if(language_cur==3)
                QMessageBox::information(this,"Error!","No programm is selected. We can't continue.");
	}
    }
    else if(state_current==2)					    //program page. Need to jump to list page...
    {
        state_current=1;                                            //set current state to programs page
	state_update(state_current);				    //and refresh it:)
    }
    addCommand(black_label,0,black_off_speed);			    //add command to undark window
}

void MainWindow::on_button_2_clicked()				    //when button 2 clicked. It is close button
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    if(state_current==1)					    //programs list
    {
	QCloseEvent *event=new QCloseEvent;
	closeEvent(event);					    //closse
    }
    else if(state_current==2)					    //program page
    {
	QCloseEvent *event=new QCloseEvent;
	closeEvent(event);					    //closse
    }
}

void MainWindow::on_button_3_clicked()				    //when button 3 clicked. It is select all on list page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    if(state_current==1)					    //if programs
    {
	for(int i=0;i<=pages_num;i++)				    //foa each program
	    for(int j=0;j<checkboxes_num;j++)
		programs[pages[i][j]-1].checked=true;		    //set checked
	checkboxes_recheck();					    //recheck current page
    }
}

void MainWindow::on_button_4_clicked()				    //when button 4 clicked. It is unselect all on list page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    if(state_current==1)					    //if programs
    {
	for(int i=0;i<=pages_num;i++)				    //foa each program
	    for(int j=0;j<checkboxes_num;j++)
		programs[pages[i][j]-1].checked=false;		    //set checked
	checkboxes_recheck();					    //recheck current page
    }
}

void MainWindow::on_button_back_clicked()			    //Navigate buttons, button back(previous page)
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    if(state_current==1)					    //if programs list
    {
	pages_clip();						    //animate
	for(int i=0;i<checkboxes_num;i++)			    //save current checks
            if(pages[page_cur][i]>0)
                programs[pages[page_cur][i]-1].checked=chbox[i].isChecked();
        page_cur--;                                                 //change page
        state_update(state_current);                                //redraw
    }
    if(state_current==2)					    //if program page
    {
	pages_clip();						    //animate
	installing_cur--;                                           //change page
	state_update(state_current);                                //redraw
    }
}

void MainWindow::on_button_next_clicked()			    //Navigate buttons, button naex(next page)
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    if(state_current==1)					    //if programs list
    {
	pages_clip();						    //animate
	for(int i=0;i<checkboxes_num;i++)			    //save current checks
            if(pages[page_cur][i]>0)
                programs[pages[page_cur][i]-1].checked=chbox[i].isChecked();
        page_cur++;                                                 //change page
        state_update(state_current);                                //redraw
    }
    if(state_current==2)					    //if program page
    {
	pages_clip();						    //animate
	installing_cur++;                                           //change page
	state_update(state_current);                                //redraw
    }
}

void MainWindow::on_button_save_clicked()			    //button to save checks on list page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    save_list->setCurrentRow(-1);				    //unselect items in save command list
    save_list->move(this->mapFromGlobal(QCursor::pos()).x(), this->mapFromGlobal(QCursor::pos()).y()-save_menu_size_y);//move to cursor position
    save_list->show();						    //show list
    save_list->raise();						    //bring to front
}

void MainWindow::on_button_search_clicked()			    //button to search on list page
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    for(int i=0;i<checkboxes_num;i++)				    //save current checks
	if(pages[page_cur][i]>0)
	    programs[pages[page_cur][i]-1].checked=chbox[i].isChecked();
    if(search_mode==false)					    //if not search mode, make search
    {
        QString reqest;
        if(language_cur==1)
            reqest=QInputDialog::getText(this, "Найти", "Введите текст для поиска", QLineEdit::Normal, search_last);//shoq dialog
        if(language_cur==2)
            reqest=QInputDialog::getText(this, "Знайти", "Введіть текст для пошуку", QLineEdit::Normal, search_last);//shoq dialog
        if(language_cur==3)
            reqest=QInputDialog::getText(this, "Serch", "Enter text to find", QLineEdit::Normal, search_last);//shoq dialog
        search(reqest);
    }
    else if(search_mode==true)					    //if serch mode off, need to turn off it
    {
	for(int i=0;i<pages_max_num;i++)  //foa each program	    //restore full backup of pages variable
	    for(int j=0;j<checkboxes_max_num;j++)
		pages[i][j]=search_pages_tmp[i][j];
	pages_num=search_pages_num_tmp;				    //restore pages number
	page_cur=0;						    //jump to first page
	search_mode=false;					    //turn off search mode
	state_update(state_current);				    //update interface
    }
}

void MainWindow::search(QString reqest)
{
    if(!reqest.isEmpty())					    //if not empty
    {
        search_last=reqest;					    //save reqest //search
        search_found_num=0;

        for(int i=0;i<programs_num;i++)			    //serch and save results to foubd variable
        {
            if(programs[i].name.contains(reqest,Qt::CaseInsensitive)==true)//if name contains reqest text
            {
                search_found[search_found_num]=i;                //add to massive
                search_found_num++;
            }
        }

        int r=search_found_num;                                 //sort found variable
        for(int i=0;i<search_found_num;i++)
        {
            for(int j=1;j<r;j++)
            {
                if(programs[search_found[j-1]].name>programs[search_found[j]].name)
                {
                    int tmp=search_found[j-1];
                    search_found[j-1]=search_found[j];
                    search_found[j]=tmp;
                }
            }
            r--;
        }

        for(int i=0;i<pages_max_num;i++)			    //make backup and Null pages variable
            for(int j=0;j<checkboxes_max_num;j++)
            {
                if(search_mode==false)
                    search_pages_tmp[i][j]=pages[i][j];
                pages[i][j]=0;
            }

        if(search_mode==false)
            search_pages_num_tmp=pages_num;                 //save pages number
        pages_num=0;                                	    //null pages
        int cur_checkbox=0;
        for(int i=0;i<search_found_num;i++)			    //copy found to pages variable(!!!)
        {
            pages[pages_num][cur_checkbox]=search_found[i]+1;		    //insert number of program on page
            cur_checkbox++;				    //add checkbox number
            if(cur_checkbox>=checkboxes_num)		    //if reached to the end of the page
            {
                pages_num++;				    //add page number
                cur_checkbox=0;				    //and null checkbox number
            }
        }

        page_cur=0;						    //jump to first page
        search_mode=true;					    //turn on search mode
        state_update(state_current);			    //update interface
    }
}

void MainWindow::on_pages_number_edit_returnPressed()		    //vhen user wants to jump to manual page
{
    keyboard_ignore_enter=true;
    int num=QString(ui->pages_number_edit->text()).toInt();	    //get entered number
    if(state_current==1)					    //if programs list page
    {
	if(num>=1 && num<=pages_num+1)				    //if all ok
	{
	    pages_clip();					    //animate
	    page_cur=num-1;					    //set page
	    state_update(state_current);			    //redraw
	}
	else							    //if entered page not exist
        {
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка!","Страницы под номером "+QString::number(num)+" не существует!");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка!","Сторінки з номером "+QString::number(num)+" не існує!");
            if(language_cur==3)
                QMessageBox::information(this,"Error!","No page with such number: "+QString::number(num));
	}
    }
    if(state_current==2)					    //if program page
    {
	if(num>=1 && num<=installing_selected_num)		    //if all ok
	{
	    pages_clip();					    //animate
	    installing_cur=num-1;				    //set page
	    state_update(state_current);			    //redraw
	}
	else							    //if page not exist
        {
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка!","Страницы под номером "+QString::number(num)+" не существует!");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка!","Сторінки з номером "+QString::number(num)+" не існує!");
            if(language_cur==3)
                QMessageBox::information(this,"Error!","No page with such number: "+QString::number(num));
	}
    }
}

void MainWindow::button_visible(QLabel *label, bool visible)	    //set all received buttons parameters to received state
{
    if(visible==true)						    //if need to show
    {
	par[getObjNum(label)].anim=true;			    //turn on anitaion(all buttons anumated)
	if(par[getObjNum(label)].pair!=NULL)par[getObjNum(label)].pair->show();//show pair
	label->show();						    //show object
    }
    else							    //if need to hide
    {
	par[getObjNum(label)].anim=false;			    //turn off anitaion(all buttons anumated)
	if(par[getObjNum(label)].pair!=NULL)par[getObjNum(label)].pair->hide();//hide pair
	label->hide();						    //disable tooltip
	tooltip_set(label,false,"");
    }
}

void MainWindow::state_update(int state){			    //1=list page; 2=program page
    this->setFocus();
    keyboard_mode=false;
    if(state==1)						    //program list
    {
	//------------------------------hide previous state
	installing_name.hide();					    //hide name
        installing_description.hide();				    //hide description
	button_visible(&installing_screenshot_button_back,false);   //hide screenshot
        button_visible(&installing_button_open_back, false);        //hide open button
	for(int i=0;i<installing_button_num;i++)		    //hide buttons
	    button_visible(&installing_button_back[i],false);
        installing_button_menu_list->hide();                        //hide contex menus
        save_list->hide();
        //------------------------------button text
        if(language_cur==1)
        {
            ui->button_1->setText("Далее");				    //set text for button in this state
            button_visible(ui->button_1_back,true);			    //and set if button visible
            ui->button_2->setText("Отмена");
            button_visible(ui->button_2_back,true);
            ui->button_3->setText("Выделить все");
            button_visible(ui->button_3_back,true);
            ui->button_4->setText("Снять выделение");
            button_visible(ui->button_4_back,true);

            ui->button_back->setText("Предыдущая");
            ui->button_next->setText("Следующая");
        }
        if(language_cur==2)
        {
            ui->button_1->setText("Далі");				    //set text for button in this state
            button_visible(ui->button_1_back,true);			    //and set if button visible
            ui->button_2->setText("Скасувати");
            button_visible(ui->button_2_back,true);
            ui->button_3->setText("Позначити все");
            button_visible(ui->button_3_back,true);
            ui->button_4->setText("Зняти позначки");
            button_visible(ui->button_4_back,true);

            ui->button_back->setText("Попередня");
            ui->button_next->setText("Наступна");
        }
        if(language_cur==3)
        {
            ui->button_1->setText("Continue");				    //set text for button in this state
            button_visible(ui->button_1_back,true);			    //and set if button visible
            ui->button_2->setText("Cancel");
            button_visible(ui->button_2_back,true);
            ui->button_3->setText("Select all");
            button_visible(ui->button_3_back,true);
            ui->button_4->setText("Deselect");
            button_visible(ui->button_4_back,true);

            ui->button_back->setText("Previous");
            ui->button_next->setText("Next");
        }
	//------------------------------button tooltips
        if(language_cur==1)
        {
            tooltip_set(ui->button_1,true,"Продолжить установку");	    //set tooltips for button in this state
            tooltip_set(ui->button_2,true,"Выйти из Матера");
            tooltip_set(ui->button_3,true,"Выбрать все галочки");
            tooltip_set(ui->button_4,true,"Снять все галочки");
            tooltip_set(ui->button_back,true,"Перейти на предыдущую страницу");
            tooltip_set(ui->button_next,true,"Перейти на следующую страницу");
            tooltip_set(ui->theme_box,true,"Выбрать графическую тему");
            tooltip_set(ui->language_box,true,"Выбрать язык интерфейса Мастера");
            tooltip_set(ui->pages_number_edit,true,"Текущая страница. Можно ввести вручную и нажать Enter для перехода");
            tooltip_set(ui->button_save,true,"Сохранить/Открыть отметки");
            tooltip_set(ui->button_help,true,"Открыть справочную систему");
            tooltip_set(ui->checkBox_animation,true,"Использовать плавную подсветку элементов интерфейса");
            tooltip_set(ui->checkBox_sound,true,"Озвучивать наведения и нажатия на кнопки");
            if(search_mode==false) tooltip_set(ui->button_search,true,"Поиск по списку программ");
            if(search_mode==true) tooltip_set(ui->button_search,true,"Выйти из режима поиска");
            tooltip_set(&installing_button_open,false,"Открыть папку в которой лежат устаночные файлы программы");
        }
        if(language_cur==2)
        {
            tooltip_set(ui->button_1,true,"Продовжити встановлення");	    //set tooltips for button in this state
            tooltip_set(ui->button_2,true,"Вийти з Майстра");
            tooltip_set(ui->button_3,true,"Обрати всі програми");
            tooltip_set(ui->button_4,true,"Скасувати вибір всих програм");
            tooltip_set(ui->button_back,true,"Перейти на попередню сторінку");
            tooltip_set(ui->button_next,true,"Перейти на наступну сторінку");
            tooltip_set(ui->theme_box,true,"Обрати графічну тему");
            tooltip_set(ui->language_box,true,"Обрати мову інтерфейсу Майстра");
            tooltip_set(ui->pages_number_edit,true,"Поточна сторінка. Можна набрати номер сторінки та натиснути Enter");
            tooltip_set(ui->button_save,true,"Зберегти/Відновити позначки");
            tooltip_set(ui->button_help,true,"Відкрити довідкову систему");
            tooltip_set(ui->checkBox_animation,true,"Використовувати плавну підсвітку елементів інтерфейсу");
            tooltip_set(ui->checkBox_sound,true,"Озвучувати наведення курсора миші на кнопки");
            if(search_mode==false) tooltip_set(ui->button_search,true,"Пошук по списку програм");
            if(search_mode==true) tooltip_set(ui->button_search,true,"Вийти з режима пошуку");
            tooltip_set(&installing_button_open,false,"Открыть папку в которой лежат устаночные файлы программы");
        }
        if(language_cur==3)
        {
            tooltip_set(ui->button_1,true,"Continue installing");	    //set tooltips for button in this state
            tooltip_set(ui->button_2,true,"Close Master");
            tooltip_set(ui->button_3,true,"Select all programs");
            tooltip_set(ui->button_4,true,"Deselect all programs");
            tooltip_set(ui->button_back,true,"Go to previous page");
            tooltip_set(ui->button_next,true,"Go to next page");
            tooltip_set(ui->theme_box,true,"Select graphics theme");
            tooltip_set(ui->pages_number_edit,true,"Current page. It's possible to type page number and press Enter");
            tooltip_set(ui->button_save,true,"Save/restore selecting");
            tooltip_set(ui->button_help,true,"Open help");
            tooltip_set(ui->checkBox_animation,true,"Use smooth highlighting for buttons");
            tooltip_set(ui->checkBox_sound,true,"Play sound when highlighting buttons");
            if(search_mode==false) tooltip_set(ui->button_search,true,"Search through the list of programs");
            if(search_mode==true) tooltip_set(ui->button_search,true,"Exit Search");
            tooltip_set(&installing_button_open,false,"Открыть папку в которой лежат устаночные файлы программы");
        }
	//------------------------------checkboxes
	for(int i=0;i<checkboxes_num;i++)			    //for each checkbox
	{
	    int cur_prog_index=pages[page_cur][i];		    //gt program index
	    if(cur_prog_index!=0)				    //if information about this checkbox is not empty
	    {
		if(cur_prog_index>0)				    //if it program
		{
		    cur_prog_index--;				    //derement index
		    chbox[i].setText(programs[cur_prog_index].name);//set checkbox text
		    chbox[i].setChecked(programs[cur_prog_index].checked);//set checkbox checked
                    if(programs[cur_prog_index].description_ru=="file")//if need to get description from file
		    {
			QString desc_file=programs_folder+categories[programs[cur_prog_index].category]+"\\"+programs[cur_prog_index].name+"\\"+programs_file_description;
			if(QFile::exists(desc_file))		    //get current description
			{
			    QFile vFile(desc_file);		    //open description file for reading
			    bool ok=vFile.open(QIODevice::ReadOnly);
			    if(ok==true)			    //if file opened correctly
			    {
                                QTextStream v(&vFile);		    //create ctream
                                QString desk_ru="";             //desk ru/desk ua/desk en
                                QString desk_ua="";
                                QString desk_en="";
                                QChar ch;
                                while(!v.atEnd())        		    //get description by char...
                                {
                                    v>>ch;
                                    if(ch=='/')
                                        break;
                                    desk_ru.append(ch);
                                    if(v.atEnd())
                                        break;
                                }
                                while(!v.atEnd())        		    //get description by char...
                                {
                                    v>>ch;
                                    if(ch=='/')
                                        break;
                                    desk_ua.append(ch);
                                    if(v.atEnd())
                                        break;
                                }
                                while(!v.atEnd())        		    //get description by char...
                                {
                                    v>>ch;
                                    if(ch=='/')
                                        break;
                                    desk_en.append(ch);
                                    if(v.atEnd())
                                        break;
                                }
                                vFile.close();			    //close file for reading
                                programs[cur_prog_index].description_ru=desk_ru;
                                if(desk_ua=="")
                                    programs[cur_prog_index].description_ua=desk_ru;
                                else
                                    programs[cur_prog_index].description_ua=desk_ua;
                                if(desk_en=="")
                                    programs[cur_prog_index].description_en=desk_ru;
                                else
                                    programs[cur_prog_index].description_en=desk_en;
			    }
			    else				    //if file don't opened correctly
                            {
                                    programs[cur_prog_index].description_ru="Не удалось открыть файл с описанием";
                                    programs[cur_prog_index].description_ua="Не вдалося відкрити файл з описом";
                                    programs[cur_prog_index].description_en="Could not open a file with a description";
			    }
			}
			else					    //if file don't exist
                        {
                                programs[cur_prog_index].description_ru="Файл с описанием не найден";
                                programs[cur_prog_index].description_ua="Файл з описом не знайдено";
                                programs[cur_prog_index].description_en="Description file not found";
			}
                    }
                    if(language_cur==1)
                        tooltip_set(&chbox[i],true,programs[cur_prog_index].description_ru);//set checkbox tooltip
                    if(language_cur==2)
                        tooltip_set(&chbox[i],true,programs[cur_prog_index].description_ua);//set checkbox tooltip
                    if(language_cur==3)
                        tooltip_set(&chbox[i],true,programs[cur_prog_index].description_en);//set checkbox tooltip
		    setFont(&chbox[i], checkbox_font_family, checkbox_font_size, 0, 0, 0);//set checksox font
		    QObject::disconnect(&chbox[i], SIGNAL(clicked()), this, SLOT(category_check()));//disconntct from signals connected when it wag group
		}
		if(cur_prog_index<0)				    //if it is category
		{
		    cur_prog_index++;				    //increment index
		    chbox[i].setText(categories[-cur_prog_index]);  //set text for checkbox
                    chbox[i].setChecked(category_checked(-cur_prog_index));//decide and set check
                    if(language_cur==1)
                        tooltip_set(&chbox[i],true,"Группа");	    //set tooltip
                    if(language_cur==2)
                        tooltip_set(&chbox[i],true,"Група");	    //set tooltip
                    if(language_cur==3)
                        tooltip_set(&chbox[i],true,"Group");	    //set tooltip
		    QObject::connect(&chbox[i],SIGNAL(clicked()),this,SLOT(category_check()));//connect for refreshing checkbox when selected
		    setFont(&chbox[i], checkbox_font_family, checkbox_font_size, 1, 0, 0);//set font
		}
		setColor(&chbox[i], theme_color_checkboxes_normal, theme_color_checkboxes_hover, theme_color_checkboxes_pressed);//set checkbox color according to selected theme
		chbox[i].show();				    //show checkbox
	    }
	    else						    //if information about this checkbox is empty
	    {
		chbox[i].hide();				    //hide checkbox
		tooltip_set(&chbox[i],false,"");		    //deactivate tooltip
	    }
	}
	//------------------------------interface
	if(page_cur<=0)						    //lock/unlock buttons to prevent going to unexisting page
	    ui->button_back->setEnabled(false);
	else
	    ui->button_back->setEnabled(true);
	if(page_cur>=pages_num)
	    ui->button_next->setEnabled(false);
	else
	    ui->button_next->setEnabled(true);
	if(search_mode==true)					    //set pages number label
        {
            if(language_cur==1)
                ui->pages_number_text->setText("Стр.                 из "+QString::number(pages_num+1)+" ("+QString::number(search_found_num)+")");
            if(language_cur==2)
                ui->pages_number_text->setText("Стор.                з "+QString::number(pages_num+1)+" ("+QString::number(search_found_num)+")");
            if(language_cur==3)
                ui->pages_number_text->setText("Page                 of "+QString::number(pages_num+1)+" ("+QString::number(search_found_num)+")");
        }
	else
        {
            if(language_cur==1)
                ui->pages_number_text->setText("Стр.                 из "+QString::number(pages_num+1)+" ("+QString::number(programs_num)+")");
            if(language_cur==2)
                ui->pages_number_text->setText("Стор.                з "+QString::number(pages_num+1)+" ("+QString::number(programs_num)+")");
            if(language_cur==3)
                ui->pages_number_text->setText("Page                 of "+QString::number(pages_num+1)+" ("+QString::number(programs_num)+")");
        }
	ui->pages_number_edit->setText(QString::number(page_cur+1));
	button_visible(ui->button_search_back, true);		    //set serch and save buttons
        if(search_mode==true)                                       //if search mode on
	{
	    par[getObjNum(ui->button_search_back)].img=search_button_serch_mode;//set info about it in database
	    ui->button_search_back->setPixmap(opacity(QPixmap(search_button_serch_mode), par[getObjNum(ui->button_search_back)].opac_normal));//refresh label on page
	}
	if(search_mode==false)					    //if search mode off
	{
	    par[getObjNum(ui->button_search_back)].img=search_button_noral_mode;//set info about it in database
	    ui->button_search_back->setPixmap(opacity(QPixmap(search_button_noral_mode), par[getObjNum(ui->button_search_back)].opac_normal));//refresh label on page
	}
        button_visible(ui->button_save_back, true);		    //show save button
        save_list->clear();
        if(language_cur==1)
        {
            save_list->addItem("Открыть");
            save_list->addItem("Сохранить");
            save_list->addItem("Отменить");
            if(search_mode==false)
                ui->label_help->setText("Выберите программы которые Вы собираетесь установить и нажмите кнопку Далее. \nДля перехода по страницам используйте \"Следующая\"/\"Предыдущая\".");
            if(search_mode==true)
                ui->label_help->setText("Поиск по запросу \""+search_last+"\".");
        }
        if(language_cur==2)
        {
            save_list->addItem("Відкрити");
            save_list->addItem("Зберегти");
            save_list->addItem("Відмінити");
            if(search_mode==false)
                ui->label_help->setText("Оберіть програми які Ви збираєтесь встановити та натисніть кнопку Далі. \nДля переходу по сторінкам використовуйте \"Наступна \" / \"Попередня \".");
            if(search_mode==true)
                ui->label_help->setText("Пошук за запитом \""+search_last+"\".");
        }
        if(language_cur==3)
        {
            save_list->addItem("Open");
            save_list->addItem("Save");
            save_list->addItem("Cancel");
            if(search_mode==false)
                ui->label_help->setText("Select the programs you are going to install and click Continue. \nFor move between pages, use the \"Next\" / \"Previous\".");
            if(search_mode==true)
                ui->label_help->setText("Search by reqest \""+search_last+"\".");
        }
    }
    if(state==2)						    //program page
    {
	//------------------------------hide previous state
	for(int i=0;i<checkboxes_num;i++)                           //hide all the checkboxes
	{
	    chbox[i].hide();
	    tooltip_set(&chbox[i], false, "");
	}
	button_visible(ui->button_search_back,false);		    //hide buttons
        button_visible(ui->button_save_back,false);
        installing_button_menu_list->hide();                        //hide contex menus
        save_list->hide();
        //------------------------------button text
        if(language_cur==1)
        {
            ui->button_1->setText("Назад");				    //set text for buttons
            ui->button_2->setText("Выйти");
            ui->button_back->setText("Предыдущая");
            ui->button_next->setText("Следующая");
            installing_button_open.setText("Открыть папку программы");
        }
        if(language_cur==2)
        {
            ui->button_1->setText("Назад");				    //set text for buttons
            ui->button_2->setText("Вийти");
            ui->button_back->setText("Попередня");
            ui->button_next->setText("Наступна");
            installing_button_open.setText("Відкрити папку програми");
        }
        if(language_cur==3)
        {
            ui->button_1->setText("Back");				    //set text for buttons
            ui->button_2->setText("Exit");
            ui->button_back->setText("Previous");
            ui->button_next->setText("Next");
            installing_button_open.setText("Open program folder");
        }
        button_visible(ui->button_1_back,true);			    //and set if button visible
	button_visible(ui->button_2_back,true);
        button_visible(ui->button_3_back,false);
        button_visible(ui->button_4_back,false);

        //------------------------------button tooltips
        if(language_cur==1)
        {
            tooltip_set(ui->button_1,true,"Вернуться к списку программ");//set tooltip for each button
            tooltip_set(ui->button_2,true,"Выйти из Матера");
            tooltip_set(ui->button_3,false,"Выбрать все галочки");
            tooltip_set(ui->button_4,false,"Снять все галочки");
            tooltip_set(ui->button_back,true,"Перейти на предыдущую программу");
            tooltip_set(ui->button_next,true,"Перейти на следующую программу");
            tooltip_set(ui->theme_box,true,"Выбрать графическую тему");
            tooltip_set(ui->pages_number_edit,true,"Текущая страница. Можно ввести вручную и нажать Enter для перехода");
            tooltip_set(ui->button_help,true,"Открыть справочную систему");
            tooltip_set(ui->checkBox_animation,true,"Использовать плавную подсветку элементов интерфейса");
            tooltip_set(ui->checkBox_sound,true,"Озвучивать наведения и нажатия на кнопки");
            tooltip_set(ui->button_save,false,"Сохранить/восстановить отметок");
            tooltip_set(ui->button_search,false,"Поиск по списку программ");
            tooltip_set(&installing_button_open,true,"Открыть папку в которой лежат устаночные файлы программы");
        }
        if(language_cur==2)
        {
            tooltip_set(ui->button_1,true,"Повернутися до сторінки зі списком");//set tooltip for each button
            tooltip_set(ui->button_2,true,"Вийти з Майстра");
            tooltip_set(ui->button_3,false,"");
            tooltip_set(ui->button_4,false,"");
            tooltip_set(ui->button_save,false,"");
            tooltip_set(ui->button_search,false,"");
            tooltip_set(ui->button_back,true,"Перейти до попередньої програми");
            tooltip_set(ui->button_next,true,"Перейти до наступної програми");
            tooltip_set(ui->theme_box,true,"Обрати графічну тему");
            tooltip_set(ui->pages_number_edit,true,"Поточна сторінка. Можна набрати номер сторінки та натиснути Enter");
            tooltip_set(ui->button_help,true,"Відкрити довідкову систему");
            tooltip_set(ui->checkBox_animation,true,"Використовувати плавну підсвітку елементів інтерфейсу");
            tooltip_set(ui->checkBox_sound,true,"Озвучувати наведення курсора миші на кнопки");
            tooltip_set(&installing_button_open,true,"Відкрити папку в якій лежать файли встановлення цієї програми");
        }
        if(language_cur==3)
        {
            tooltip_set(ui->button_1,true,"Back to program list");//set tooltip for each button
            tooltip_set(ui->button_2,true,"Close Master");
            tooltip_set(ui->button_3,false,"");
            tooltip_set(ui->button_4,false,"");
            tooltip_set(ui->button_save,false,"");
            tooltip_set(ui->button_search,false,"");
            tooltip_set(ui->button_back,true,"Go to previous page");
            tooltip_set(ui->button_next,true,"Go to next page");
            tooltip_set(ui->theme_box,true,"Select graphics theme");
            tooltip_set(ui->pages_number_edit,true,"Current page. It's possible to type page number and press Enter");
            tooltip_set(ui->button_help,true,"Open help");
            tooltip_set(ui->checkBox_animation,true,"Use smooth highlighting for buttons");
            tooltip_set(ui->checkBox_sound,true,"Play sound when highlighting buttons");
            tooltip_set(&installing_button_open,true,"Open folder when stored installer");
        }
	//------------------------------show new elements
	for(int i=0;i<installing_button_num;i++)		    //show all the buttons
	{
	    installing_button[i].show();
	    installing_button_back[i].show();
	    par[getObjNum(&installing_button_back[i])].anim=1;
	}
	par[getObjNum(&installing_screenshot_button_back)].anim=1;  //show screenshot
	installing_screenshot_button_back.show();
	installing_screenshot_button.show();
	installing_name.show();					    //show name
        installing_description.show();				    //show descrition
        button_visible(&installing_button_open_back, true);
	//------------------------------apply program info
        installing_name.setText(programs[installing_selected[installing_cur]].name);//set name
        if(language_cur==1)
            installing_description.setText(programs[installing_selected[installing_cur]].description_ru);//set description
        if(language_cur==2)
            installing_description.setText(programs[installing_selected[installing_cur]].description_ua);//set description
        if(language_cur==3)
            installing_description.setText(programs[installing_selected[installing_cur]].description_en);//set description
	QString Fname=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs_file_screenshot;//create screenshot file name
	par[getObjNum(&installing_screenshot_button_back)].img=Fname;//update info about screenshot in database
	installing_screenshot_button_back.setPixmap(opacity(QPixmap(Fname).scaled(installing_screenshot_button_back.size()),par[getObjNum(&installing_screenshot_button_back)].opac_normal));//set screenshot pixmap
        //------------------------------update interface
        if(language_cur==1)
        {
            ui->pages_number_text->setText("Стр.                 из "+QString::number(installing_selected_num));//set pages number
            ui->label_help->setText("Устанавливайте выбранные на первом этапе программы по очереди. \nДля перехода по программам используйте \"Следующая\"/\"Предыдущая\".");
        }
        if(language_cur==2)
        {
            ui->pages_number_text->setText("Стор.                з "+QString::number(installing_selected_num));//set pages number
            ui->label_help->setText("Встановлюйте вибрані на першому етапі програми по черзі. \nДля переходу по програмам використовуйте \"Наступна \" / \"Попередня \".");
        }
        if(language_cur==3)
        {
            ui->pages_number_text->setText("Page                 of "+QString::number(installing_selected_num));//set pages number
            ui->label_help->setText("Install selected programs one-by-one. \nFor move between programs, use the \"Next\" / \"Previous\".");
        }
        ui->pages_number_edit->setText(QString::number(installing_cur+1));

	if(installing_cur<=0)					    //lock/unlock buttons to prevent going to unexisting page
	    ui->button_back->setEnabled(false);
	else
	    ui->button_back->setEnabled(true);
	if(installing_cur>=installing_selected_num-1)
	    ui->button_next->setEnabled(false);
	else
	    ui->button_next->setEnabled(true);
	for(int i=0;i<installing_button_num;i++)		    //process buttons
	{
	    if(i<programs[installing_selected[installing_cur]].files.count())//if needed
	    {
		button_visible(&installing_button_back[i], true);
		QString FullName=programs[installing_selected[installing_cur]].files[i];
		QString OnlyName="";
                for(int l=0;l<FullName.length() && FullName[l]!='.';l++)
                    OnlyName.append(FullName[l]);
                if(OnlyName.length()>installing_button_text_length)     //cut the name if its too long
                {
                    OnlyName=OnlyName.mid(0,installing_button_text_length-2);
                    OnlyName.append("...");
                }
		installing_button[i].setText(OnlyName);
                tooltip_set(&installing_button_back[i], true, FullName);
	    }
	    else							    //if don't neednt
	    {
		button_visible(&installing_button_back[i], false);
	    }
	}
	QObject::disconnect(&installing_button[0],SIGNAL(clicked()),this,SLOT(installing_button_1_clicked()));
	QObject::connect(&installing_button[0],SIGNAL(clicked()),this,SLOT(installing_button_1_clicked()));//connect buttons

	QObject::disconnect(&installing_button[1],SIGNAL(clicked()),this,SLOT(installing_button_2_clicked()));
	QObject::connect(&installing_button[1],SIGNAL(clicked()),this,SLOT(installing_button_2_clicked()));

	QObject::disconnect(&installing_button[2],SIGNAL(clicked()),this,SLOT(installing_button_3_clicked()));
	QObject::connect(&installing_button[2],SIGNAL(clicked()),this,SLOT(installing_button_3_clicked()));

	QObject::disconnect(&installing_button[3],SIGNAL(clicked()),this,SLOT(installing_button_4_clicked()));
	QObject::connect(&installing_button[3],SIGNAL(clicked()),this,SLOT(installing_button_4_clicked()));

	QObject::disconnect(&installing_button[4],SIGNAL(clicked()),this,SLOT(installing_button_5_clicked()));
	QObject::connect(&installing_button[4],SIGNAL(clicked()),this,SLOT(installing_button_5_clicked()));

	QObject::disconnect(&installing_screenshot_button,SIGNAL(clicked()),this,SLOT(installing_button_screenshot_clicked()));
        QObject::connect(&installing_screenshot_button,SIGNAL(clicked()),this,SLOT(installing_button_screenshot_clicked()));
        installing_button_menu_list->clear();
        if(language_cur==1)
        {
            installing_button_menu_list->addItem("Открыть");
            installing_button_menu_list->addItem("Добавить в очередь");
            installing_button_menu_list->addItem("Отменить");
        }
        if(language_cur==2)
        {
            installing_button_menu_list->addItem("Відкрити");
            installing_button_menu_list->addItem("Додати до черги");
            installing_button_menu_list->addItem("Відмінити");
        }
        if(language_cur==3)
        {
            installing_button_menu_list->addItem("Open");
            installing_button_menu_list->addItem("Add to quere");
            installing_button_menu_list->addItem("Cancel");
        }
    }
    //on all pages
    quere_menu_list->clear();
    if(language_cur==1)
    {
        quere_menu_list->addItem("Открыть файл");
        quere_menu_list->addItem("Убрать из очереди");
        quere_menu_list->addItem("Отмена");
        ui->button_about->setText("О программе");
        ui->checkBox_animation->setText("Анимация");
        ui->checkBox_sound->setText("Звуки");
        ui->label_text_language->setText("Язык:");
        ui->label_text_theme->setText("Тема:");
    }
    if(language_cur==2)
    {
        quere_menu_list->addItem("Відкрити файл");
        quere_menu_list->addItem("Прибрати з черги");
        quere_menu_list->addItem("Відміна");
        ui->button_about->setText("Про програму");
        ui->checkBox_animation->setText("Анімація");
        ui->checkBox_sound->setText("Звуки");
        ui->label_text_language->setText("Мова:");
        ui->label_text_theme->setText("Тема:");
    }
    if(language_cur==3)
    {
        quere_menu_list->addItem("Open file");
        quere_menu_list->addItem("Remove from quere");
        quere_menu_list->addItem("Cancel");
        ui->button_about->setText("About");
        ui->checkBox_animation->setText("Animation");
        ui->checkBox_sound->setText("Sounds");
        ui->label_text_language->setText("Language:");
        ui->label_text_theme->setText("Theme:");
    }
    quere_update();
}

void MainWindow::apply_theme(int theme_id)			    //load info to variables
{
    this->setFocus();
    theme_cur=theme_id;
    parameter_set("theme",themes[theme_cur]);			    //set name of theme
    par[getObjNum(ui->background)].img=folder_themes+themes[theme_cur]+theme_file_background;//set image filenames to objects
    par[getObjNum(ui->button_1_back)].img=folder_themes+themes[theme_cur]+theme_file_button;
    par[getObjNum(ui->button_2_back)].img=folder_themes+themes[theme_cur]+theme_file_button;
    par[getObjNum(ui->button_3_back)].img=folder_themes+themes[theme_cur]+theme_file_button;
    par[getObjNum(ui->button_4_back)].img=folder_themes+themes[theme_cur]+theme_file_button;
    par[getObjNum(ui->button_next_back)].img=folder_themes+themes[theme_cur]+theme_file_nextback;
    par[getObjNum(ui->button_back_back)].img=folder_themes+themes[theme_cur]+theme_file_nextback;
    for(int i=0;i<installing_button_num;i++)
        par[getObjNum(&installing_button_back[i])].img=folder_themes+themes[theme_cur]+theme_file_button;

    FILE* col=fopen(QString(folder_themes+themes[theme_cur]+theme_file_colors).toAscii(),"r");//get colors table
    if(col!=NULL)						    //if file opened correctly
    {
	char ch;						    //symbol 10=end of line
	for(int i=0;i<theme_file_colors_lines_num && !feof(col);i++)
	{
	    *theme_file_colors_lines_aim[i]="";			    //get variables(it stores) in the adresses massive
	    ch=fgetc(col);
	    while(ch!=10 && ch!=' ' && !feof(col))		    //sign ' ' may be used to write comment
	    {
		theme_file_colors_lines_aim[i]->append(QChar::fromAscii(ch));
		ch=fgetc(col);
	    }
	    while(ch!=10 && !feof(col))ch=fgetc(col);		    //skip anything before end of line
	}
        fclose(col);
    }
    else							    //if file don't opened correctly
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка!","Не удается открыть файл с таблицей цветов: "+folder_themes+themes[theme_cur]+theme_file_colors);
        if(language_cur==2)
            QMessageBox::information(this,"Помилка!","Не вдалося відкрити файл з таблицею кольорів: "+folder_themes+themes[theme_cur]+theme_file_colors);
        if(language_cur==3)
            QMessageBox::information(this,"Error!","Can't open file with colors table: "+folder_themes+themes[theme_cur]+theme_file_colors);
    }
    apply_graphics();						    //apply info from variables
}

void MainWindow::quere_update()					    //update quere
{
    for(int i=0;i<quere_items_max_num;i++)			    //for each item
    {
	if(i<quere_items_num)					    //if exists
        {
	    button_visible(&quere_back_label[i], true);		    //show
	    quere_button[i].setText(quere[i].name +" \n "+quere[i].file);//load
        }
	else							    //if don't exist
        {
	    button_visible(&quere_back_label[i], false);	    //hide
        }
    }
    quere_menu_list->hide();
}

void MainWindow::quere_menu_show()
{
    quere_menu_list->setCurrentRow(-1);				    //unselect items in save command list
    quere_menu_list->move(this->mapFromGlobal(QCursor::pos()).x(), this->mapFromGlobal(QCursor::pos()).y());//move to cursor position
    quere_menu_list->show();						    //show list
    quere_menu_list->raise();
}

void MainWindow::quere_item_1_clicked()				    //when quere item clicked
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    quere_last_button_clicked=0;
    QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
    if(QFile(path).exists())
    {
        QUrl url;

        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::quere_item_2_clicked()				    //when quere item clicked
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    quere_last_button_clicked=1;
    QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
    if(QFile(path).exists())
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::quere_item_3_clicked()				    //when quere item clicked
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    quere_last_button_clicked=2;
    QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
    if(QFile(path).exists())
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::quere_item_4_clicked()				    //when quere item clicked
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    quere_last_button_clicked=3;
    QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
    if(QFile(path).exists())
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::quere_item_5_clicked()				    //when quere item clicked
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    quere_last_button_clicked=4;
    QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
    if(QFile(path).exists())
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::quere_item_6_clicked()				    //when quere item clicked
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    quere_last_button_clicked=5;
    QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
    if(QFile(path).exists())
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
        if(language_cur==2)
            QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
        if(language_cur==3)
            QMessageBox::information(this,"Error","File "+path+" not found.");
    }
}

void MainWindow::quere_menu_selected(QModelIndex index)		    //when one of the items of quere right-click menu selected
{
    QString selected=index.data().toString();			    //make string of selected item
    quere_menu_list->hide();					    //hide list
    if(selected=="Открыть файл" || selected=="Відкрити файл" || selected=="Open file")				    //if selected open command
    {
        QString path=programs_folder+quere[quere_last_button_clicked].category+"\\"+quere[quere_last_button_clicked].name+"\\"+quere[quere_last_button_clicked].file;
        if(QFile(path).exists())
        {
            QUrl url;
            url.clear();
            url=QUrl(path);
            QDesktopServices::openUrl(url);
        }
        else
        {
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
            if(language_cur==3)
                QMessageBox::information(this,"Error","File "+path+" not found.");
        }
    }
    if(selected=="Убрать из очереди" || selected=="Прибрати з черги" || selected=="Remove from quere")				    //if selected open command
    {
        for(int i=quere_last_button_clicked;i<(quere_items_num-1);i++)
        {
            quere[i]=quere[i+1];
        }
        quere_items_num--;
        quere_update();
    }
}

void MainWindow::apply_graphics()				    //load graphics from variables
{
    for(int i=0;par[i].exis==true;i++)				    //apply button manual parameters
    {
	(par[i].obj)->setMouseTracking(true);                       //set tracking
	if(par[i].pair!=NULL)(par[i].pair)->setMouseTracking(true); //set pair tracking
	if(QFile::exists(par[i].img))
	{
	    if(par[i].img_buffer_filename!=par[i].img)              //if buffered not correct image file
            {
		par[i].img_buffer=QPixmap(par[i].img).scaled(par[i].obj->size());//load new scaled image to buffer
		par[i].img_buffer_filename=par[i].img;              //set info about buffered image
            }
	    if(par[i].pair!=NULL)(par[i].pair)->resize((par[i].obj)->size());//resize pair ad main object
	    if(par[i].pair!=NULL)(par[i].pair)->move((par[i].obj)->pos());//move pair
	    (par[i].obj)->setPixmap(opacity(par[i].img_buffer,par[i].opac));//set image to object
	}
	else							    //if image from variable don't exist
	{
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка","Файл "+par[i].img+" не найден.");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка","Файл "+par[i].img+" не знайдено.");
            if(language_cur==3)
                QMessageBox::information(this,"Error","File "+par[i].img+" not found.");
        }
    }
    for(int i=0;i<checkboxes_num;i++)				    //apply color for checkboxes
        setColor(&chbox[i],theme_color_checkboxes_normal,theme_color_checkboxes_hover,theme_color_checkboxes_pressed);
    setColor(ui->button_1,theme_color_buttons_normal,theme_color_buttons_hover,theme_color_buttons_pressed);//apply color to main buttons
    setColor(ui->button_2,theme_color_buttons_normal,theme_color_buttons_hover,theme_color_buttons_pressed);
    setColor(ui->button_3,theme_color_buttons_normal,theme_color_buttons_hover,theme_color_buttons_pressed);
    setColor(ui->button_4,theme_color_buttons_normal,theme_color_buttons_hover,theme_color_buttons_pressed);
    for(int i=0;i<installing_button_num;i++)                        //apply color for buttons on the program page
        setColor(&installing_button[i],theme_color_buttons_normal,theme_color_buttons_hover,theme_color_buttons_pressed);
    setColor(ui->label_copyright,theme_color_copyright, theme_color_copyright, theme_color_copyright);//apply color to other different elements
    setColor(ui->label_help, theme_color_help_text, theme_color_help_text, theme_color_help_text);
    setColor(ui->button_update, theme_color_headbuttons_normal, theme_color_headbuttons_hover, theme_color_headbuttons_pressed);
    setColor(ui->label_text_theme, theme_color_help_text, theme_color_help_text, theme_color_help_text);
    setColor(ui->label_text_language, theme_color_help_text, theme_color_help_text, theme_color_help_text);
    setColor(ui->button_about, theme_color_headbuttons_normal, theme_color_headbuttons_hover, theme_color_headbuttons_pressed);
    setColor(ui->checkBox_animation, theme_color_headbuttons_normal, theme_color_headbuttons_hover, theme_color_headbuttons_pressed);
    setColor(ui->checkBox_sound, theme_color_headbuttons_normal, theme_color_headbuttons_hover, theme_color_headbuttons_pressed);
    setColor(ui->button_next,theme_color_nextback_normal,theme_color_nextback_hover,theme_color_nextback_pressed);
    setColor(ui->button_back,theme_color_nextback_normal,theme_color_nextback_hover,theme_color_nextback_pressed);
}

void MainWindow::category_check()				    //check all the members of the received category number
{
    for(int i=0;i<checkboxes_num;i++)				    //save current checks
	if(pages[page_cur][i]>0)
	    programs[pages[page_cur][i]-1].checked=chbox[i].isChecked();
    QPoint cursor_position=this->mapFromGlobal(QCursor::pos());	    //get mouse position
    int mx=cursor_position.x();
    int my=cursor_position.y();
    for(int i=0;i<checkboxes_num;i++)				    //for every checkbox
    {
	if(pages[page_cur][i]<0)				    //if it group
	{
	    if ((mx>chbox[i].x()) && (mx<chbox[i].x()+chbox[i].width()) && (my<chbox[i].y()+chbox[i].height()) && (my>chbox[i].y()))/*Курсор на объекте*/
	    {
		bool value=chbox[i].isChecked();		    //get group value
		for(int j=0;j<programs_num;j++)			    //go to all programs
		{
		    if(programs[j].category==-(pages[page_cur][i]+1))//if this program of needed catgory
		    {
			programs[j].checked=value;		    //apply value
		    }
		}
	    }
	}
    }
    checkboxes_recheck();					    //recheck checkboxes
}

void MainWindow::onTimer()					    //anytime animation
{
    for(int i=0;commands[i].exist==true;i++)			    //button animation
    {
	//------get info
	int dbObjNum=getObjNum(commands[i].object);		    //get number of current object
	int current=par[dbObjNum].opac;				    //get current object opacity
	int after=current;					    //firstly set arter as current
	int step=commands[i].step;				    //get step
	//------desicion(plus or minus and how much)
	bool complete=false;					    //if aim reached
	if(commands[i].aim>current)				    //if need to plus
	{
	    after=current+(step);				    //make plus
	    if(after>=commands[i].aim)				    //if now current mucher than aim - set current to aim
	    {
		after=commands[i].aim;
		complete=true;					    //mark command as complete
	    }
	}
	if(commands[i].aim<=current)				    //if need to minus
	{
	    after=current-step;					    //make minus
	    if(after<commands[i].aim)				    //if now current less than aim - set current to aim
	    {
                after=commands[i].aim;
		complete=true;					    //mark command as complete
	    }
	}
	//------refresh buffer
	if(par[dbObjNum].img!=par[dbObjNum].img_buffer_filename)    //if buffered not correct image
        {
            par[dbObjNum].img_buffer=QPixmap(par[dbObjNum].img).scaled(par[dbObjNum].obj->size());//refresh buffer
	    par[dbObjNum].img_buffer_filename=par[dbObjNum].img;//refresh buffer info
	}
	(*commands[i].object).setPixmap(opacity(par[dbObjNum].img_buffer,after));//apply buffer image

	par[dbObjNum].opac=after;				    //refresh information in database
	if(commands[i].object->isVisible()==true)		    //if visible
	{
	    if(after<=0)					    //and now not
		commands[i].object->hide();			    //hide
	}
	else							    //if not visible`
	{
	    if(after>0 && (par[dbObjNum].anim==true || commands[i].object==black_label))//but now visible and must be animated
		commands[i].object->show();			    //show
	}
	if(complete)						    //erace command if complete
	{
	    if(commands[i].object==black_label && commands[i].aim>10)//if black label turned on - stop animation
		animation->stop();
	    for(int j=i;j<max_commands_num-1 && commands[j].exist==true;j++)//move all massive
		commands[j]=commands[j+1];
	    commands[max_commands_num-1].exist=false;		    //erase last element
	    i--;						    //skip one iteration after eracing
	}
    }
}

void MainWindow::tooltips_on_timer()				    //timer controls animation
{
    if(this->isActiveWindow()==true)				    //if this is active window
    {
	QPoint cursor_position=this->mapFromGlobal(QCursor::pos()); //get cursor position
	int mx=cursor_position.x();
	int my=cursor_position.y();
	for(int i=0;i<objects_num && tooltips[i].exist==true;i++)   //for every command
	{
	    if ((mx>tooltips[i].object->x()) && (mx<tooltips[i].object->x()+tooltips[i].object->width()) && (my<tooltips[i].object->y()+tooltips[i].object->height()) && (my>tooltips[i].object->y()))/*Курсор на объекте*/
	    {
		if(tooltips_last_hover[i]==0)			    /*курсор только что попад на объект*/
		{
		    tooltips_last_hover[i]=1;			    /*Курсор уже на объекте*/
		}
		else						    //курсор и до этого был на объекте
		{
		    if(tooltips_last_hover[i]==tooltip_delay)	    //курсор находится на объекте достаточное время
		    {
			QToolTip::showText(QCursor::pos(),tooltips[i].text, this);
		    }
		    if(tooltips_last_hover[i]<100)		    //увелисиваем время нахождения курсора на объекте, но только если оно еще не превысило 100
			tooltips_last_hover[i]++;
		}
		break;						    //нашли объект - завершаем цикл.
	    }
	    else						    //курсор на объекте
	    {
		if(tooltips_last_hover[i]>0)			    //курсор ушел с объекта
		{
		    tooltips_last_hover[i]=0;			    /*Курсор уже не на объекте*/
		}
	    }
	}
    }
}

void MainWindow::on_show()					    //при показе
{
    //--------------------------show black
    black_label->setPixmap(opacity(QPixmap(black_imagefile),par[getObjNum(black_label)].opac));
    black_label->show();
    //--------------------------initialize temp labla's
    QLabel *loading_label=new QLabel;				    //label to indicate that something loading
    this->layout()->addWidget(loading_label);
    setColor(loading_label, "ffffff", "ffffff", "ffffff");
    setFont(loading_label,(QString)"Arial",36,0,0,0);
    loading_label->move(300,210);
    loading_label->resize(300,100);
    loading_label->setText("Loading...");
    loading_label->raise();
    loading_label->show();
    QLabel *status_label=new QLabel;				    //label to indicate what loading
    this->layout()->addWidget(status_label);
    setColor(status_label, "999999", "999999", "999999");
    setFont(status_label,(QString)"Arial",11,0,0,0);
    status_label->move(450,290);
    status_label->resize(200,30);
    status_label->setText("Loading...");
    status_label->raise();
    status_label->show();
    sleep(50);							    //чтобы успело перерисоваться окно
    //-------------------------initialize big massives
    status_label->setText("Initialize big massives...");
    QApplication::processEvents();
    for(int page=0;page<pages_max_num;page++)			    //initialize pages
        for(int prog=0;prog<checkboxes_max_num;prog++)
            pages[page][prog]=0;
    for(int i=0;i<programs_max_num;i++)				    //initialize programs
        programs[i].checked=false;
    //----------------------quere
    status_label->setText("Processing quere...");
    QApplication::processEvents();
    for(int i=0;i<quere_items_max_num;i++)
    {
        this->layout()->addWidget(&quere_back_label[i]);
        this->layout()->addWidget(&quere_button[i]);
        button_visible(&quere_back_label[i], false);
        quere_back_label[i].resize(quere_item_size_x, quere_item_size_y);
        quere_button[i].resize(quere_back_label[i].size());
        quere_back_label[i].move(quere_position_x, quere_position_y+(quere_item_size_y+quere_item_interval_y)*i);
        quere_button[i].move(quere_back_label[i].pos());
        quere_back_label[i].setPixmap(QPixmap(quere_back_file));
        quere_button[i].setFlat(true);
        quere_button[i].setFocusPolicy(Qt::NoFocus);
    }
    QObject::connect(&quere_button[0],SIGNAL(clicked()),this, SLOT(quere_item_1_clicked()));
    QObject::connect(&quere_button[1],SIGNAL(clicked()),this, SLOT(quere_item_2_clicked()));
    QObject::connect(&quere_button[2],SIGNAL(clicked()),this, SLOT(quere_item_3_clicked()));
    QObject::connect(&quere_button[3],SIGNAL(clicked()),this, SLOT(quere_item_4_clicked()));
    QObject::connect(&quere_button[4],SIGNAL(clicked()),this, SLOT(quere_item_5_clicked()));
    QObject::connect(&quere_button[5],SIGNAL(clicked()),this, SLOT(quere_item_6_clicked()));
    quere_items_num= parameter_get("quere_len").toInt();            //get quere
    for(int i=0;i<quere_items_num;i++)
    {
        quere[i].category=parameter_get("quere_item_"+QString::number(i)+"_category");
        quere[i].file=parameter_get("quere_item_"+QString::number(i)+"_file");
        quere[i].name=parameter_get("quere_item_"+QString::number(i)+"_name");
    }
    //----------------------language
    status_label->setText("Setting language list...");
    QApplication::processEvents();
    ui->language_box->addItem("Русский", 1);
    ui->language_box->addItem("Українська", 2);
    ui->language_box->addItem("English", 3);
    ui->language_box->setCurrentIndex(language_cur-1);
    QObject::connect(ui->language_box,SIGNAL(currentIndexChanged(int)),this,SLOT(onLanguageBox_changed(int)));//соединяем изменение темы в списке с обновлением темы
    //----------------------themes
    status_label->setText("Getting theme list...");
    QApplication::processEvents();
    QDir dir(folder_themes);					    //get theme list
    QStringList dirs;
    dirs=dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    themes_num=dirs.count();
    for(int i=0;i<themes_num;++i)				    //set info to combobox
    {
        ui->theme_box->addItem(dirs[i],i);
	themes[i]=dirs[i];
    }
    theme_cur=0;
    if(themes_num>0)						    //get last theme and restore
    {
	QString themename;					    //deside theme name
	if(parameter_exist("theme"))				    //если параметр с именем темы есть то ставид данные их параметра.
	    themename=parameter_get("theme");
	else							    //иначе ставим стандартную тему
	    themename=theme_basic;
	for(int i=0;i<themes_num;i++)				    //find desided name. Else setting ups first theme
	{
	    if(themes[i]==themename)				    //тема найдена
	    {
		theme_cur=i;					    //задаем ее
		break;						    //нашли. Выходим с цикла
	    }
        }
	ui->theme_box->setCurrentIndex(theme_cur);		    //определяем тему в списке тем
    }
    else
    {
        if(language_cur==1)
            QMessageBox::information(this,"Информация","В папке " + folder_themes + " найдено ни одной темы.");
        if(language_cur==2)
            QMessageBox::information(this,"Інформація","В папці " + folder_themes + " не знайдено жодної теми.");
        if(language_cur==3)
            QMessageBox::information(this,"Info","No themes found in " + folder_themes + " folder.");
    }
    QObject::connect(ui->theme_box,SIGNAL(currentIndexChanged(int)),this,SLOT(onThemeBox_changed(int)));//соединяем изменение темы в списке с обновлением темы
    //---------------getting categories and programs
    status_label->setText("Getting program list...");
    QApplication::processEvents();
    categories=QDir(programs_folder).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);//считывваем список категорий
    categories_num=categories.count();				    //считаем категории
    programs_num=0;
    for(int category=0;category<categories_num;category++)	    //parse categories
    {
	QStringList progs=QDir(programs_folder+categories[category]+"\\").entryList(QDir::AllDirs | QDir::NoDotAndDotDot);//формируем список программ в категории
	for(int prog=0;prog<progs.count();prog++)		    //process programs
	{
	    programs[programs_num].name=progs[prog];		    //задать имя
	    programs[programs_num].category=category;		    //определить категорию
	    programs[programs_num].checked=false;		    //пока еще никто ничего не выбирал
            programs[programs_num].description_ru="file";		    //говорим что описание где то в файле, сейчас его читать не стоит
	    programs[programs_num].files.clear();
	    QStringList files=QDir(programs_folder+categories[category]+"\\"+programs[programs_num].name).entryList(QDir::Files);//считываем список файлов
	    for(int file=0;file<files.count();file++)               //add installer as first
		if(QString(files[file]).contains(programs_file_installer, Qt::CaseInsensitive))
		    programs[programs_num].files.append(files[file]);
	    for(int file=0;file<files.count();file++)               //add other files без установщика, скриншота и описания
		if(!(QString(files[file]).contains(programs_file_installer, Qt::CaseInsensitive)) && files[file]!=programs_file_screenshot && files[file]!=programs_file_description)
		    programs[programs_num].files.append(files[file]);
	    programs_num++;					    //да, программ сатло на одну больше
	}
    }
    if(programs_num==0)
    {
        if(language_cur==1)
            QMessageBox::information(this, "Обратите внимание", "В папке "+programs_folder+" не найдено ни одной программы.");
        if(language_cur==2)
            QMessageBox::information(this, "Зверніть увагу", "В папці "+programs_folder+" не знайдено жодної програми.");
        if(language_cur==3)
            QMessageBox::information(this, "Attention", "No programs found in "+programs_folder+" folder.");
    }
    //-----------------creating checkboxes
    status_label->setText("Creating checkboxes...");		    //определяем чекбоксы как объекты
    QApplication::processEvents();
    checkboxes_num=0;
    for(int curX=checkboxes_left;curX<checkboxes_right-checkbox_size_x;curX+=checkbox_size_x+checkbox_interval_x)//будем делать их пока место не кончится!
    {
	for(int curY=checkboxes_top;curY<checkboxes_bottom-checkbox_size_y;curY+=checkbox_size_y+checkbox_interval_y)//Заполняем столбец
        {
            this->layout()->addWidget(&chbox[checkboxes_num]);	    //добавляем на форму
	    chbox[checkboxes_num].move(curX,curY);		    //задаем место
            chbox[checkboxes_num].resize(checkbox_size_x,checkbox_size_y);//размер
            chbox[checkboxes_num].setFocusPolicy(Qt::NoFocus);
            chbox[checkboxes_num].hide();                           //спрятать чтобы не муляло глаза раньше времени
	    checkboxes_num++;					    //и продолжаем=)
        }
    }
    //-----------------creating installing page
    status_label->setText("Creating installing page...");           //buttons
    for(int i=0;i<installing_button_num;i++)			    //обработаем каждую кнопку!
    {
        button_visible(&installing_button_back[i], false);
        this->layout()->addWidget(&installing_button_back[i]);	    //добавим на форму
        this->layout()->addWidget(&installing_button[i]);
        installing_button[i].setFlat(true);			    //сделаем прозрачными!
        installing_button_back[i].resize(installing_button_size_x,installing_button_size_y);//зададим размер!
        installing_button[i].resize(installing_button_back[i].size());
	installing_button[i].setMouseTracking(true);		    //зададим управления мышью!
        installing_button_back[i].setMouseTracking(true);
        setFont(&installing_button[i],"MS Shell Dlg 2",12,false,false,false);//укажeм шрифт
	installing_button_back[i].move(checkboxes_left+5,installing_name_size_y+checkboxes_top+((installing_button_interval_y+installing_button_size_y)*i)+5);//переместим!
        installing_button[i].move(installing_button_back[i].pos());
        installing_button[i].setText("button "+QString::number(i)); //даже укажeм текст
        installing_button[i].setFocusPolicy(Qt::NoFocus);
    }
    button_visible(&installing_screenshot_button_back , false);       //screenshot
    this->layout()->addWidget(&installing_screenshot_button_back);
    this->layout()->addWidget(&installing_screenshot_button);
    installing_screenshot_button.setFlat(true);
    installing_screenshot_button_back.setMouseTracking(true);
    installing_screenshot_button.setMouseTracking(true);
    installing_screenshot_button_back.move(checkboxes_right-installing_screenshot_button_size_x-5,checkboxes_bottom-installing_screenshot_button_size_y-5);
    installing_screenshot_button.move(installing_screenshot_button_back.pos());
    installing_screenshot_button_back.resize(installing_screenshot_button_size_x,installing_screenshot_button_size_y);
    installing_screenshot_button.resize(installing_screenshot_button_back.size());
    installing_screenshot_button_back.setPixmap(QPixmap(folder_graphics+"\\"+installing_screenshot_file_default));
    installing_screenshot_button.setFocusPolicy(Qt::NoFocus);

    this->layout()->addWidget(&installing_name);
    installing_name.resize(checkboxes_right-checkboxes_left, installing_name_size_y);
    installing_name.move(checkboxes_left, checkboxes_top);
    installing_name.setMouseTracking(true);                         //name
    setFont(&installing_name, installing_name_font_name, installing_name_font_size, false, false, false);
    installing_name.setAlignment(Qt::AlignCenter);
    installing_name.setText("//NAME");
    installing_name.hide();

    this->layout()->addWidget(&installing_description);
    installing_description.setMouseTracking(true);                  //description
    installing_description.move(checkboxes_right-installing_description_size_x, checkboxes_top+installing_name_size_y+installing_button_interval_y);
    installing_description.resize(installing_description_size_x, installing_description_size_y);
    setFont(&installing_description, installing_description_font_name, installing_description_font_size, false, false, false);
    installing_description.setAlignment(Qt::AlignLeft);
    installing_description.setText("//DESCRIPTION");
    installing_description.setWordWrap(true);
    installing_description.hide();

    button_visible(&installing_button_open_back, false);       //screenshot
    this->layout()->addWidget(&installing_button_open_back);
    this->layout()->addWidget(&installing_button_open);
    installing_button_open.setFlat(true);
    installing_button_open.setMouseTracking(true);
    installing_button_open_back.setMouseTracking(true);
    installing_button_open_back.move(installing_button_open_x, installing_button_open_y);
    installing_button_open.move(installing_button_open_back.pos());
    installing_button_open_back.resize(installing_button_open_size_x, installing_button_open_size_y);
    installing_button_open.resize(installing_button_open_back.size());
    installing_button_open_back.setPixmap(QPixmap(theme_file_button));
    installing_button_open.setFocusPolicy(Qt::NoFocus);
    QObject::connect(&installing_button_open, SIGNAL(clicked()), this, SLOT(installing_button_open_clicked()));
    //-----------------generating pages
    status_label->setText("Generating pages...");
    QApplication::processEvents();
    int cur_prog=0;
    int cur_page=0;
    pages_num=0;
    for(int category=0;category<categories_num;category++)          //process categories
    {
	pages[cur_page][cur_prog]=-(category+1);		    //создаем чекбокс-группу
	cur_prog++;
	if(cur_prog>=checkboxes_num)				    //если надо, переключить страницу
	{
	    cur_page++;
	    pages_num++;
	    cur_prog=0;
	}
	for(int program=0;program<programs_num;program++)	    //process all programs to find cur categoty
	{
	    if(programs[program].category==category)		    //если нужная категория,
	    {
		pages[cur_page][cur_prog]=(program+1);		    //добавить на страницу
		cur_prog++;
		if(cur_prog>=checkboxes_num)			    //если надо, переключить страницу
		{
		    cur_page++;
		    pages_num++;
		    cur_prog=0;
		}
	    }
	}
    }
//    //-----------------checking updates
//    status_label->setText("Checking updates...");
//    QApplication::processEvents();
//    QObject::connect(&update_http, SIGNAL(done(bool)), this, SLOT(update_done(bool)));
//    QObject::connect(&update_http, SIGNAL(dataReadProgress(int,int)), this, SLOT(update_read_progress(int,int)));
//    update_file.setFileName(update_download_to);
//    bool ok=update_file.open(QFile::WriteOnly);
//    if(ok)
//    {
//        update_http.setHost(update_download_host, QHttp::ConnectionModeHttp, 80);
//        update_http.get(update_download_file, &update_file);
//    }
    //-----------------update interface
    status_label->setText("Update interface...");
    QApplication::processEvents();
    state_update(state_current);
    if(parameter_exist("animation"))                                //get info about animation
    {
        QString data=parameter_get("animation");
        if(data=="true")
            ui->checkBox_animation->setChecked(true);
        else
            ui->checkBox_animation->setChecked(false);
    }
    else
        ui->checkBox_animation->setChecked(true);
    if(parameter_exist("sound"))                                    //get info about sound
    {
        QString data=parameter_get("sound");
        if(data=="true")
            ui->checkBox_sound->setChecked(true);
        else
            ui->checkBox_sound->setChecked(false);
    }
    else
        ui->checkBox_sound->setChecked(true);
    if(language_cur==1)
    {
        QString text="Не найдены следующие звуковые файлы:";            //check and inform user about unexisting sound files
        bool all_files_exists=true;
        if(!QFile(audio_button_hover_sound_file).exists())
        {
            all_files_exists=false;
            text.append("\n "+audio_button_hover_sound_file);
        }
        if(!QFile(audio_button_click_sound_file).exists())
        {
            all_files_exists=false;
            text.append("\n "+audio_button_click_sound_file);
        }
        if(!all_files_exists)
        {
            QMessageBox::information(this,"Внимание!", text);
        }
    }
    if(language_cur==2)
    {
        QString text="Не знайдені наступні звукові файли:";            //check and inform user about unexisting sound files
        bool all_files_exists=true;
        if(!QFile(audio_button_hover_sound_file).exists())
        {
            all_files_exists=false;
            text.append("\n "+audio_button_hover_sound_file);
        }
        if(!QFile(audio_button_click_sound_file).exists())
        {
            all_files_exists=false;
            text.append("\n "+audio_button_click_sound_file);
        }
        if(!all_files_exists)
        {
            QMessageBox::information(this,"Увага!", text);
        }
    }
    if(language_cur==3)
    {
        QString text="Following audio files not found:";            //check and inform user about unexisting sound files
        bool all_files_exists=true;
        if(!QFile(audio_button_hover_sound_file).exists())
        {
            all_files_exists=false;
            text.append("\n "+audio_button_hover_sound_file);
        }
        if(!QFile(audio_button_click_sound_file).exists())
        {
            all_files_exists=false;
            text.append("\n "+audio_button_click_sound_file);
        }
        if(!all_files_exists)
        {
            QMessageBox::information(this,"Attention!", text);
        }
    }
    black_label->raise();
    loading_label->raise();
    status_label->raise();
    //-----------------apply theme
    status_label->setText("Applying theme...");
    QApplication::processEvents();
    if(themes_num>0)						    //применить тему только если есть что применять
	apply_theme(theme_cur);
    else
	apply_graphics();
    //------------------hiding labels
    status_label->setText("Hiding labels...");
    QApplication::processEvents();
    loading_label->hide();
    status_label->hide();
    delete status_label;
    delete loading_label;
    addCommand(black_label,0,black_off_speed);
    //---------------------test move
    //sleep(2000);
    //move(ui->button_1_back,400,400,1000);
}

void MainWindow::installing_button_menu_on_selected_item(QModelIndex index)
{
    QString selected=index.data().toString();			    //make string of selected item
    installing_button_menu_list->hide();
    if(selected=="Открыть" || selected=="Open" || selected=="Відкрити")
    {
        QString path=programs_folder+categories[programs[installing_selected[installing_cur]].category]+"\\"+programs[installing_selected[installing_cur]].name+"\\"+programs[installing_selected[installing_cur]].files[installing_button_last_right_click_number];
        if(QFile(path).exists())
        {
            QUrl url;
            url.clear();
            url=QUrl(path);
            QDesktopServices::openUrl(url);
        }
        else
        {
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка","Файл "+path+" не найден.");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка","Файл "+path+" не знайдено.");
            if(language_cur==3)
                QMessageBox::information(this,"Error","File "+path+" not found.");
        }
    }
    if(selected=="Добавить в очередь" || selected=="Додати до черги" || selected=="Add to quere")
    {
        if(quere_items_num<quere_items_max_num)
        {
            quere[quere_items_num].name=programs[installing_selected[installing_cur]].name;
            quere[quere_items_num].category=categories[programs[installing_selected[installing_cur]].category];
            quere[quere_items_num].file=programs[installing_selected[installing_cur]].files[installing_button_last_right_click_number];
            quere_items_num++;
            quere_update();
        }
        else
        {
            if(language_cur==1)
                QMessageBox::information(this,"Ошибка", "Очередь заполнена");
            if(language_cur==2)
                QMessageBox::information(this,"Помилка", "Черга заповнена");
            if(language_cur==3)
                QMessageBox::information(this,"Error", "The quere if full");
        }
    }
}

void MainWindow::save_on_selected_item(QModelIndex index)	    //Пункт из меню сохранения выбран
{
    QString selected=index.data().toString();			    //make string of selected item
    save_list->hide();						    //hide list
    if(selected=="Открыть" || selected=="Відкрити" || selected=="Open")					    //if selected open command
    {
        open();
    }
    else if(selected=="Сохранить" || selected=="Зберегти" || selected=="Save")				    //if selected save item
    {
        save();
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);						    //setup fucking ui
    alr1=false;
    alr2=false;
    for(int i=0;i<objects_num;i++)				    //initialize hover variable
	last_hover[i]=false;
    for(int i=0;i<objects_num;i++)				    //initialize tooltips hover variable
	tooltips_last_hover[i]=0;
    for(int i=0;i<objects_num;i++)				    //initialize tooltips variable
	tooltips[i].exist=false;
    for(int i=0;i<max_commands_num;i++)				    //initialize commands
        commands[i].exist=false;
    //----------parameters---------------
    state_current=1;						    //1=list page; 2=program page
    version="??.?";
    //black screensaver parameters
    black_label=new QLabel;
    black_max=200;
    black_imagefile=":\\black.png";
    black_on_speed=20;
    black_off_speed=40;
    //folders
    folder_themes="Data\\Themes\\";
    folder_audio="Data\\Audio\\";
    folder_graphics="Data\\Graphics\\";
    folder_windows=QString::fromAscii(getenv("WinDir"))+"\\";
    folder_desktop=QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)+"\\";
    folder_appdata=QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"\\";
    file_parameters=folder_appdata+"FP_SoftPack.conf";
    file_version="Data\\v";
    file_help_ru="Data\\help_ru.chm";
    file_help_en="Data\\help_en.chm";
    file_help_ua="Data\\help_ua.chm";
    //themes
    theme_basic="Стандарт";
    theme_color_buttons_normal="004400";
    theme_color_buttons_hover="000000";
    theme_color_buttons_pressed="ffff00";
    theme_color_nextback_normal="000000";
    theme_color_nextback_hover="000000";
    theme_color_nextback_pressed="000000";
    theme_color_header="000000";
    theme_color_help_text="000000";
    theme_color_pages="000000";
    theme_color_headbuttons_normal="000000";
    theme_color_headbuttons_hover="000000";
    theme_color_headbuttons_pressed="000000";
    theme_color_copyright="000000";
    theme_color_checkboxes_normal="004400";
    theme_color_checkboxes_hover="000000";
    theme_color_checkboxes_pressed="ffff00";
    theme_color_radiobutton_normal="000000";
    theme_color_radiobutton_hover="000000";
    theme_color_radiobutton_pressed="000000";

    theme_file_colors_lines_num=19;				    //set adresses of variables which will be get from file
    theme_file_colors_lines_aim[0]=&theme_color_buttons_normal;
    theme_file_colors_lines_aim[1]=&theme_color_buttons_hover;
    theme_file_colors_lines_aim[2]=&theme_color_buttons_pressed;
    theme_file_colors_lines_aim[3]=&theme_color_nextback_normal;
    theme_file_colors_lines_aim[4]=&theme_color_nextback_hover;
    theme_file_colors_lines_aim[5]=&theme_color_nextback_pressed;
    theme_file_colors_lines_aim[6]=&theme_color_header;
    theme_file_colors_lines_aim[7]=&theme_color_help_text;
    theme_file_colors_lines_aim[8]=&theme_color_pages;
    theme_file_colors_lines_aim[9]=&theme_color_headbuttons_normal;
    theme_file_colors_lines_aim[10]=&theme_color_headbuttons_hover;
    theme_file_colors_lines_aim[11]=&theme_color_headbuttons_pressed;
    theme_file_colors_lines_aim[12]=&theme_color_copyright;
    theme_file_colors_lines_aim[13]=&theme_color_checkboxes_normal;
    theme_file_colors_lines_aim[14]=&theme_color_checkboxes_hover;
    theme_file_colors_lines_aim[15]=&theme_color_checkboxes_pressed;
    theme_file_colors_lines_aim[16]=&theme_color_radiobutton_normal;
    theme_file_colors_lines_aim[17]=&theme_color_radiobutton_hover;
    theme_file_colors_lines_aim[18]=&theme_color_radiobutton_pressed;

    theme_file_background="\\back.png";
    theme_file_button="\\button.png";
    theme_file_nextback="\\nextback.png";
    theme_file_colors="\\colors.ini";
    //checkboxes
    checkboxes_top=114;
    checkboxes_bottom=500;
    checkboxes_left=245;
    checkboxes_right=769;
    checkbox_size_x=254;//254
    checkbox_size_y=15;
    checkbox_interval_x=15;
    checkbox_interval_y=1;//1
    checkbox_font_size=10;
    checkbox_font_family="MS Shell Dlg 2";
    //buttons
    button_font_size=12;
    button_font_name="MS Shell Dlg 2";
    button_font_bold=false;
    button_font_italic=false;
    button_font_underlined=false;
    //window
    window_title="FP SoftPack";
    window_size_X=790;
    window_size_Y=547;
    //tooltips
    tooltips_timer_delay=100;
		    //tooltips list in state_update function
    tooltip_delay=9;
    //programs
    programs_folder="Soft\\";
    programs_num=0;
    programs_file_screenshot="Скриншот.png";
    programs_file_installer="Установка";
    programs_file_description="Описание.txt";
    program_cur=0;
    //pages
    pages_num=0;
    page_cur=0;
    //installing
    installing_screenshot_button_size_x=250;
    installing_screenshot_button_size_y=150;
    installing_screenshot_file_default="screenshot_default.png";
    installing_selected_num=0;
    installing_cur=0;
    installing_button_size_x=186;
    installing_button_size_y=34;
    installing_button_interval_y=10;
    //------------------------------------------------
    installing_button_open_x=570;
    installing_button_open_y=310;
    installing_button_open_size_x=180;
    installing_button_open_size_y=25;
    //------------------------------------------------
    installing_name_size_y=30;
    installing_name_font_size=20;
    installing_name_font_name="MS Shell Dlg 2";
    installing_description_font_size=12;
    installing_description_font_name="MS Shell Dlg 2";
    installing_description_size_x=300;
    installing_description_size_y=200;
    installing_button_menu_size_x=150;
    installing_button_menu_size_y=55;
    installing_button_menu_list=new QListWidget(this);
    installing_button_menu_list->hide();
    installing_button_menu_list->resize(installing_button_menu_size_x, installing_button_menu_size_y);
    installing_button_text_length=20;
    QObject::connect(installing_button_menu_list, SIGNAL(clicked(QModelIndex)), this, SLOT(installing_button_menu_on_selected_item(QModelIndex)));
    //search
    search_mode=false;
    search_button_noral_mode=":\\search_button.png";
    search_button_serch_mode=":\\cancel_button.png";
    //save
    save_file_extension="*.fsp";
    save_directory=folder_desktop;
    save_menu_size_x=150;
    save_menu_size_y=55;
    save_list= new QListWidget(this);
    save_list->hide();
    save_list->resize(save_menu_size_x, save_menu_size_y);
    QObject::connect(save_list, SIGNAL(clicked(QModelIndex)), this, SLOT(save_on_selected_item(QModelIndex)));
    //quere
    quere_items_num=0;
    quere_item_size_x=210;
    quere_item_size_y=30;
    quere_item_interval_y=3;
    quere_position_x=12;
    quere_position_y=330;
    quere_back_file=":\\quere_back.png";
    quere_menu_size_x=200;
    quere_menu_size_y=55;
    quere_menu_list= new QListWidget(this);
    quere_menu_list->hide();
    quere_menu_list->resize(quere_menu_size_x, quere_menu_size_y);
    QObject::connect(quere_menu_list, SIGNAL(clicked(QModelIndex)), this, SLOT(quere_menu_selected(QModelIndex)));
    //audio
    audio_button_hover_sound_file=folder_audio+"hover.wav";
    audio_button_click_sound_file=folder_audio+"click.wav";
    //update
    update_download_host="fsoft-p.narod.ru";
    update_download_file="/news.txt";
    update_download_to=folder_appdata+"fpsoftpack_news.txt";
    ui->button_update->hide();
    //language
    language_cur=1;
    //keyboard
    keyboard_cur_index=0;
    keyboard_ctrl=false;
    keyboard_ignore_enter=false;
    //objects
    for(int i=0;i<objects_num;i++)par[i].exis=false;/*initialize temp parameters*/int t=-1; //ready...
    t++; par[t].obj=ui->background;        par[t].pair=NULL;               par[t].exis=1; par[t].anim=0; par[t].opac_hovered=0;   par[t].opac_normal=0;   par[t].opac=255;         par[t].img=":\\back.png";          par[t].img_buffer_filename="NULL";
    t++; par[t].obj=black_label;           par[t].pair=NULL;               par[t].exis=1; par[t].anim=0; par[t].opac_hovered=0;   par[t].opac_normal=0;   par[t].opac=black_max;   par[t].img=black_imagefile;        par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->main_area;         par[t].pair=NULL;               par[t].exis=1; par[t].anim=1; par[t].opac_hovered=250; par[t].opac_normal=90;  par[t].opac=90;          par[t].img=":\\main_area.png";     par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_1_back;     par[t].pair=ui->button_1;       par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=":\\button.png";        par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_2_back;     par[t].pair=ui->button_2;       par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=":\\button.png";        par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_3_back;     par[t].pair=ui->button_3;       par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=":\\button.png";        par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_4_back;     par[t].pair=ui->button_4;       par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=":\\button.png";        par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_save_back;  par[t].pair=ui->button_save;    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=":\\save_button.png";   par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_search_back;par[t].pair=ui->button_search;  par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=search_button_noral_mode;par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_help_back;  par[t].pair=ui->button_help;    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100; par[t].opac=100;         par[t].img=":\\help_button.png";   par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_back_back;  par[t].pair=ui->button_back;    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=150; par[t].opac_normal=70;  par[t].opac=70;          par[t].img=":\\nextback.png";      par[t].img_buffer_filename="NULL";
    t++; par[t].obj=ui->button_next_back;  par[t].pair=ui->button_next;    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=150; par[t].opac_normal=70;  par[t].opac=70;          par[t].img=":\\nextback.png";      par[t].img_buffer_filename="NULL";
    t++; par[t].obj=&installing_screenshot_button_back; par[t].pair=&installing_screenshot_button;    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=60;  par[t].opac=60;          par[t].img=folder_graphics+"\\"+installing_screenshot_file_default;      par[t].img_buffer_filename="NULL";
    t++; par[t].obj=&installing_button_open_back; par[t].pair=&installing_button_open;    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=60;  par[t].opac=60;          par[t].img=":\\button.png";      par[t].img_buffer_filename="NULL";
    for(int i=0;i<installing_button_num;i++)
    {
        t++; par[t].obj=&installing_button_back[i];  par[t].pair=&installing_button[i];    par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=60;  par[t].opac=60;          par[t].img=":\\button.png";      par[t].img_buffer_filename="NULL";
    }
    for(int i=0;i<quere_items_max_num;i++)
    {
        t++; par[t].obj=&quere_back_label[i]; par[t].pair=&quere_button[i]; par[t].exis=1; par[t].anim=1; par[t].opac_hovered=255; par[t].opac_normal=100;  par[t].opac=100;          par[t].img=quere_back_file;      par[t].img_buffer_filename="NULL";
    }
    //----------end parameters---------------
    if(parameter_exist("language"))                                 //get language
    {
        language_cur=parameter_get("language").toInt();
        if(language_cur!=1 && language_cur!=2 && language_cur!=3)
            language_cur=1;
    }
    else
        language_cur=1;
    QFile vFile(file_version);					    //get current version
    bool ok=vFile.open(QIODevice::ReadOnly);
    if(ok==true)						    //if file opened correctly
    {
	QTextStream v(&vFile);
	char ver[20];
	v>>ver;
	vFile.close();
	version=QString::fromAscii(ver);
    }
    else							    //if file don't opened correctly
    {
        if(language_cur==1)
            QMessageBox::information(this, "Ошибка!", "Не удалось открыть для чтения файл "+file_version+" содержащий версию программы. Далее программа может работать некорректно.");
        if(language_cur==2)
            QMessageBox::information(this, "Помилка!", "Не вдалося відкрити для читання файл "+file_version+" що містить версію програми. Далі програма може працювати некоректно.");
        if(language_cur==3)
            QMessageBox::information(this, "Error!", "Could not open file for reading" + file_version + "containing a version of the program. Next, the program may not work correctly.");
    }
    this->setWindowTitle(window_title+" "+version);		    //window title
    this->setWindowIcon(QIcon(":\\icon.png"));                      //window icon

    animation=new QTimer(this);					    //create timer for button animation
    QObject::connect(animation,SIGNAL(timeout()),this,SLOT(onTimer()));
    animation->start(timer_rate);

    tooltips_timer=new QTimer(this);				    //create timer for tooltips
    QObject::connect(tooltips_timer,SIGNAL(timeout()),this,SLOT(tooltips_on_timer()));
    tooltips_timer->start(tooltips_timer_delay);

    centralWidget()->setMouseTracking(true);			    //mouse tracking
    this->setMouseTracking(true);
    this->setWindowIcon(QIcon(":\\icon.png"));			    //icon
    ui->background->resize(this->width(),this->height());	    //background
    ui->background->move(0,0);
    ui->substrate->resize(this->width(),this->height());	    //substrate
    ui->substrate->move(0,0);
    black_label->move(0,0);					    //black pixmap
    black_label->resize(this->width(),this->height());
    this->layout()->addWidget(black_label);
    this->setFixedSize(window_size_X, window_size_Y);		    //set size of window
}

void MainWindow::mouseMoveEvent(QMouseEvent *m)			    //hover/leave detector
{
    int mx=m->x()-3;						    //get mouse coordinates
    int my=m->y()-3;
    for(int i=0;par[i].exis==true;i++)				    //checkking and processing objects from massive
    {
	if(par[i].anim==true)
	{
	    if ( (mx >  par[i].obj->x()) && (mx <  par[i].obj->x()+ par[i].obj->width()) && (my <  par[i].obj->y()+ par[i].obj->height()) && (my >  par[i].obj->y()) )/*Курсор на объекте*/
	    {
		if(last_hover[i]==false)			    /*ON HOVER*/
		{
                    last_hover[i]=true;				    /*Курсор уже на объекте*/
                    if(ui->checkBox_sound->isChecked())
                        if(QFile(audio_button_hover_sound_file).exists())
                            QSound::play(audio_button_hover_sound_file);//play sound
                    bool anim=ui->checkBox_animation->isChecked();  //get anim state
                    if(anim==true)                                  //if on
                    {
                        addCommand(par[i].obj, par[i].opac_hovered, 50);
                    }
                    else                                            //if off
                    {
                        addCommand(par[i].obj, par[i].opac_hovered, abs(par[i].opac_hovered-par[i].opac)+10);
                    }
		}
	    }
	    else
	    {
		if(last_hover[i]==true)				    /*ON LEAVE*/
		{
		    last_hover[i]=false;			    /*Курсор уже не на объекте*/
                    bool anim=ui->checkBox_animation->isChecked();  //get anim state
                    if(anim==true)                                  //if on
                    {
                        addCommand(par[i].obj, par[i].opac_normal,10);
                    }
                    else                                            //if off
                    {
                        addCommand(par[i].obj, par[i].opac_normal, abs(par[i].opac_normal-par[i].opac)+10);
                    }
		}
	    }
	}
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Qt::MouseButton button = event->button();			    //get button
    if(button==Qt::RightButton)					    //for right button(menu)
    {
        if(state_current==2)                                        //on program page
        {
	    for(int i=0;i<programs[installing_selected[installing_cur]].files.count();i++)//check all programs page buttons
            {
                if(event->x()>installing_button[i].x() && event->x()<installing_button[i].x()+installing_button[i].width() && event->y()>installing_button[i].y() && event->y()<installing_button[i].y()+installing_button[i].height())
                {
		    installing_button_last_right_click_number=i;    //save number for function
		    installing_button_menu_list->setCurrentRow(-1); //unselect items in save command list
                    installing_button_menu_list->move(this->mapFromGlobal(QCursor::pos()).x(), this->mapFromGlobal(QCursor::pos()).y());//move to cursor position
		    installing_button_menu_list->show();	    //show list
		    installing_button_menu_list->raise();	    //move on top
                }
            }
        }
        for(int i=0;i<quere_items_num;i++)                          //right click for quere
        {
            if(event->x()>quere_button[i].x() && event->x()<quere_button[i].x()+quere_button[i].width() && event->y()>quere_button[i].y() && event->y()<quere_button[i].y()+quere_button[i].height())
            {
		quere_last_button_clicked=i;			    //save number for function
		quere_menu_show();				    //show menu
            }
        }
    }
    if(button==Qt::LeftButton)
    {
	if(save_list->isVisible()==true)
	{
	    if(!(event->x()>save_list->x() && event->x()<save_list->x()+save_list->width() && event->y()>save_list->y() && event->y()<save_list->y()+save_list->height()))
	    {							    //if cursor not on menu
		save_list->hide();
	    }
	}
	if(installing_button_menu_list->isVisible()==true)
	{
	    if(!(event->x()>installing_button_menu_list->x() && event->x()<installing_button_menu_list->x()+installing_button_menu_list->width() && event->y()>installing_button_menu_list->y() && event->y()<installing_button_menu_list->y()+installing_button_menu_list->height()))
	    {							    //if cursor not on menu
		installing_button_menu_list->hide();
	    }
	}
	if(quere_menu_list->isVisible()==true)
	{
	    if(!(event->x()>quere_menu_list->x() && event->x()<quere_menu_list->x()+quere_menu_list->width() && event->y()>quere_menu_list->y() && event->y()<quere_menu_list->y()+quere_menu_list->height()))
	    {							    //if cursor not on menu
		quere_menu_list->hide();
	    }
	}
    }
}

MainWindow::~MainWindow()
{
    delete ui;							    //очистка оперативной памяти
    delete[] chbox;
    delete black_label;
    delete animation;
    QApplication::exit(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{								    //При закрытии затемнение окна
    addCommand(black_label,black_max,black_on_speed*2);
    update_http.abort();
    sleep(timer_rate*((black_max)/(black_on_speed*2))+50);
    parameter_set("quere_len",QString::number(quere_items_num));    //save quere
    for(int i=0;i<quere_items_num;i++)
    {
        parameter_set("quere_item_"+QString::number(i)+"_category",quere[i].category);
        parameter_set("quere_item_"+QString::number(i)+"_file",quere[i].file);
        parameter_set("quere_item_"+QString::number(i)+"_name",quere[i].name);
    }
    bool anim=ui->checkBox_animation->isChecked();                  //save anim state
    if(anim==true)
        parameter_set("animation","true");
    else
        parameter_set("animation","false");
    bool sound=ui->checkBox_sound->isChecked();                     //save sound state
    if(sound==true)
        parameter_set("sound","true");
    else
        parameter_set("sound","false");
    parameter_set("language", QString::number(language_cur));       //save current language
    QApplication::exit();
}

void MainWindow::on_button_about_clicked()                          //show info about softpack
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);            //play sound
    QMessageBox box;
    if(language_cur==1)
    {
        box.setWindowTitle("О программе");
        box.setText("<h3>FP SoftPack "+version+"</h3><br>   &nbsp;&nbsp;&nbsp;<u>FP SoftPack</u> - это комплекс <br>различного программного обеспечения,<br> необходимого при работе с компьютером.  <br>&nbsp; <u>Сайт:</u> <a href=\"http://vk.com/fpsoftpack\">vk.com/fpsoftpack</a><br>&nbsp; <u>E-mail:</u> <b>Fsoft-p@narod.ru</b><br><br><u>Разработчик:</u> Dr. Failov<br><u>Также принимали участие в разработке:</u> <br>Гуталин, Litoksa, Sinner<br><br><br>   Fsoft Production 2013");
    }
    if(language_cur==2)
    {
        box.setWindowTitle("Про програму");
        box.setText("<h3>FP SoftPack "+version+"</h3><br>   &nbsp;&nbsp;&nbsp;<u>FP SoftPack</u> - це комплекс <br> різного програмного забезпечення, <br> необхідного при роботі з комп'ютером. <br>&nbsp; <u>Сайт:</u>  <a href=\"http://vk.com/fpsoftpack\">vk.com/fpsoftpack</a><br>&nbsp; <u>E-mail:</u> <b>Fsoft-p@narod.ru</b><br><br><u>Розробник:</u> Dr. Failov<br><u>Також брали участь в розробці:</u> <br>Гуталин, Litoksa, Sinner<br><br><br>   Fsoft Production 2013");
    }
    if(language_cur==3)
    {
        box.setWindowTitle("About");
        box.setText("<h3>FP SoftPack "+version+"</h3><br>   &nbsp;&nbsp;&nbsp;<u>FP SoftPack</u> - a set of different software, <br> necessary when working with a computer. <br>&nbsp; <u>Site:</u> <a href=\"http://vk.com/fpsoftpack\">vk.com/fpsoftpack</a><br>&nbsp; <u>E-mail:</u> <b>Fsoft-p@narod.ru</b><br><br><u>Developer:</u> Dr. Failov<br><u>Also involved in the development:</u> <br>Гуталин, Litoksa, Sinner<br><br><br>   Fsoft Production 2013");
    }
    box.setIconPixmap(QPixmap(":\\icon.png").scaled(128,128));
    box.setWindowIcon(QIcon(":\\icon.png"));
    box.exec();
}

void MainWindow::on_button_help_clicked()
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    QString path;
    if(language_cur==1)
        path=file_help_ru;
    if(language_cur==2)
        path=file_help_ua;
    if(language_cur==3)
        path=file_help_en;
    if(QFile(path).exists())
    {
        QUrl url;
        url.clear();
        url=QUrl(path);
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::update_done(bool error)
{
    if(!error)
    {
        update_file.close();
        update_file.open(QFile::ReadOnly);
        QString file;
        QTextStream stream(&update_file);
        QChar ch;
        while(!stream.atEnd())
        {
            stream>>ch;
            file.append(ch);
        }
        update_file.close();
        int i=0;
        QMessageBox* m=new QMessageBox();//-------------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        m->setText(file);
        m->show();
        while(i<file.length() && file[i]!='|')
        {
            update_ver_exception.append(file[i]);
            i++;
        }
        i++;
        while(i<file.length() && file[i]!='|')
        {
            update_text.append(file[i]);
            i++;
        }
        i++;
        while(i<file.length() && file[i]!='|')
        {
            update_site.append(file[i]);
            i++;
        }
        if(version!=update_ver_exception)
        {
            ui->button_update->show();
            ui->button_update->setText(update_text);
            tooltip_set(ui->button_update, true, "Нажмите для перехода на сайт");
        }
        else
            ui->button_update->hide();
    }
    update_file.close();
}

void MainWindow::update_read_progress(int done,int all)
{
    ui->button_update->show();
    ui->button_update->setText("Загрузка данных: " + QString::number(100*(double)done/all) + '%');
}

void MainWindow::on_button_update_clicked()
{
    if(ui->checkBox_sound->isChecked())
        if(QFile(audio_button_click_sound_file).exists())
            QSound::play(audio_button_click_sound_file);                //play sound
    QUrl url;
    url.clear();
    url=QUrl("http://"+update_site);
    QDesktopServices::openUrl(url);
}

void MainWindow::onLanguageBox_changed(int _new)
{
    language_cur=_new+1;
    state_update(state_current);
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    int key=event->key();
    if(key==16777249)//ctrl
        keyboard_ctrl=false;
}

QChar GetChar(int code)
{
    QChar result=NULL;
    if(code==81)result='q';
    if(code==87)result='w';
    if(code==69)result='e';
    if(code==82)result='r';
    if(code==84)result='t';
    if(code==89)result='y';
    if(code==85)result='u';
    if(code==73)result='i';
    if(code==79)result='o';
    if(code==80)result='p';
    if(code==65)result='a';
    if(code==83)result='s';
    if(code==68)result='d';
    if(code==70)result='f';
    if(code==71)result='g';
    if(code==72)result='h';
    if(code==74)result='j';
    if(code==75)result='k';
    if(code==76)result='l';
    if(code==90)result='z';
    if(code==88)result='x';
    if(code==67)result='c';
    if(code==86)result='v';
    if(code==66)result='b';
    if(code==78)result='n';
    if(code==77)result='m';
    if(code==32)result=' ';
    return result;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key=event->key();
    //QMessageBox::information(0," ",QString::number(key));
    if(key==16777249)//ctrl
        keyboard_ctrl=true;
    //live search
    QChar ch=GetChar(key);
    if(ch!=NULL && keyboard_ctrl==false)//any allowed letter
    {
        if(!(ch==32 && keyboard_mode==true))
        {
            if(search_mode==true)
                search(search_last+ch);
            if(search_mode==false)
                search(ch);
        }
    }
    if(key==16777264)//f1
        on_button_help_clicked();
    if(key==16777266)//f3
        on_button_search_clicked();
    if(key==16777216)//esc
    {
        if(state_current==1 && search_mode==true)
            on_button_search_clicked();
        else if(state_current==2)
            on_button_1_clicked();
    }
    if(key==16777220)//enter
    {
        if(keyboard_ignore_enter==false)
        {
            if(state_current==1)
            {
                on_button_3_clicked();      //select all and continue
                on_button_1_clicked();
            }
            else if(state_current==2)       //install quick
                installing_button_1_clicked();
        }
        else
            keyboard_ignore_enter=false;
    }
    if(key==16777219)//backspace
    {
        if(state_current==1)
        {
            if(search_mode==true)
            {
                if(search_last.length()>1)//delete last sybbol and find
                {
                    QString tmp=search_last.mid(0,search_last.length()-1);
                    search(tmp);
                }
                else        //cancel finding
                    on_button_search_clicked();
            }
        }
        else if(state_current==2)
            on_button_1_clicked();
    }
    if(key==49)//1
    {
        if(state_current==1)
            on_button_1_clicked();
        else if(state_current==2)
            if(installing_button[0].isVisible())
                installing_button_1_clicked();
    }

    if(key==50)//2
    {
        if(state_current==1)
            on_button_2_clicked();
        else if(state_current==2)
            if(installing_button[1].isVisible())
                installing_button_2_clicked();
    }
    if(key==51)//3
    {
        if(state_current==1)
            on_button_3_clicked();
        else if(state_current==2)
            if(installing_button[2].isVisible())
                installing_button_3_clicked();
    }
    if(key==52)//4
    {
        if(state_current==1)
            on_button_4_clicked();
        else if(state_current==2)
            if(installing_button[3].isVisible())
                installing_button_4_clicked();
    }
    if(key==53)//5
    {
        if(state_current==2)
            if(installing_button[4].isVisible())
                installing_button_5_clicked();
    }
    if(key==16777234)//arrow left
    {
        if(state_current==1)
        {
            if(ui->button_back->isEnabled())
            {
                on_button_back_clicked();
            }
        }
        else if(state_current==2)
        {
            if(ui->button_back->isEnabled())
            {
                on_button_back_clicked();
            }
        }
    }
    if(key==16777236)//arrow right
    {
        if(state_current==1)
        {
            if(ui->button_next->isEnabled())
            {
                on_button_next_clicked();
            }
        }
        else if(state_current==2)
        {
            if(ui->button_next->isEnabled())
            {
                on_button_next_clicked();
            }
        }
    }
    if(key==1044 || key==76)//L
    {
        if(keyboard_ctrl==true)
        {
            language_cur++;
            if(language_cur>3)
                language_cur=1;
            ui->language_box->setCurrentIndex(language_cur-1);
            state_update(state_current);
        }
    }
    if(key==1045 || key==84)//T
    {
        if(keyboard_ctrl==true)
        {
            theme_cur++;
            if(theme_cur>=themes_num)
                theme_cur=0;
            ui->theme_box->setCurrentIndex(theme_cur);
            apply_theme(theme_cur);
        }
    }
    if(key==1060 || key==65)//a
    {
        if(keyboard_ctrl==true)
        {
            ui->checkBox_animation->setChecked(!ui->checkBox_animation->isChecked());
        }
    }
    if(key==1071 || key==90)//z
    {
        if(keyboard_ctrl==true)
        {
            ui->checkBox_sound->setChecked(!ui->checkBox_sound->isChecked());
        }
    }
    if(key==1067 || key==83)//s
    {
        if(keyboard_ctrl==true)
        {
            if(state_current==1)
                save();
            else if(state_current==2)
                installing_button_screenshot_clicked();
        }
    }
    if(key==1065 || key==79)//o
    {
        if(keyboard_ctrl==true)
        {
            if(state_current==1)
                open();
        }
    }
    if(key==1040 || key==70)//f
    {
        if(keyboard_ctrl==true)
        {
            if(state_current==1)
                on_button_search_clicked();
        }

    }
    if(key==16777237)//down
    {
        if(state_current==1)
        {
            keyboard_cur_index++;
            keyboard_update_index();
        }
    }
    if(key==16777235)//up
    {
        if(state_current==1)
        {
            keyboard_cur_index--;
            keyboard_update_index();
        }
    }
    if(key==32)//space
    {
        if(state_current==1 && keyboard_mode==true)
        {
            chbox[keyboard_cur_index].setChecked(!chbox[keyboard_cur_index].isChecked());
        }
    }
}

void MainWindow::save()
{
    for(int i=0;i<checkboxes_num;i++)			    //save current checks
        if(pages[page_cur][i]>0)
            programs[pages[page_cur][i]-1].checked=chbox[i].isChecked();
    QString path;
    if(language_cur==1)
        path=QFileDialog::getSaveFileName(this, "Куда сохранить файл отметок?", folder_desktop, save_file_extension);
    if(language_cur==2)
        path=QFileDialog::getSaveFileName(this, "Куди зберегти файл позначок?", folder_desktop, save_file_extension);
    if(language_cur==3)
        path=QFileDialog::getSaveFileName(this, "Where to save selections file?", folder_desktop, save_file_extension);
    if(path!=NULL)
    {
        QFile outF;						    //открываем файл для записи
        outF.setFileName(path);
        bool ok=outF.open(QIODevice::WriteOnly);
        if(ok==true)					    //if file opened correctly
        {
            QDataStream out;				    //create stream
            out.setDevice(&outF);
            out<<version;					    //load version
            for(int i=0;i<programs_num;i++)			    //load info from progams database
            {
                if(programs[i].checked==true)
                    out<<(QChar)'1';
                if(programs[i].checked==false)
                    out<<(QChar)'0';
            }
            outF.close();					    //close file
            if(language_cur==1)
                QMessageBox::information(this, "Выполнено!", "Отметки сохранены успешно.");
            if(language_cur==2)
                QMessageBox::information(this, "Виконано!", "Позначки вдало збережені.");
            if(language_cur==3)
                QMessageBox::information(this, "Success!", "Marks saved successfully.");
        }
        else
        {							    //if file don't opened correctly
            if(language_cur==1)
                QMessageBox::critical(this, "Ошибка", "Открыть файл" +path+ "для записи не удалось. Попробуйте сохранить файл в другом месте.");
            if(language_cur==2)
                QMessageBox::critical(this, "Помилка", "Відкрити файл" +path+ "для запису не вдалося. Спробуйте зберегти файл в іншому місці.");
            if(language_cur==3)
                QMessageBox::critical(this, "Error", "Failed to open file "+path+" for writing. Try to save file in other place.");
        }
    }
}

void MainWindow::open()
{
    QString path;
    if(language_cur==1)
        path=QFileDialog::getOpenFileName(this, "Выберите файл отметок", folder_desktop, save_file_extension);//show dialog to get filename
    if(language_cur==2)
        path=QFileDialog::getOpenFileName(this, "Оберіть файл позначок", folder_desktop, save_file_extension);//show dialog to get filename
    if(language_cur==3)
        path=QFileDialog::getOpenFileName(this, "Select file", folder_desktop, save_file_extension);//show dialog to get filename
    if(path!=NULL)						    //if path not empty
    {
        QFile inF;						    //открываем файл для чтения
        inF.setFileName(path);
        bool ok=inF.open(QIODevice::ReadOnly);
        if(ok==true)					    //if opened correctly
        {
            QDataStream in;					    //create stream to read from
            in.setDevice(&inF);
            QString file_version;
            in>>file_version;				    //get file version
            if(file_version==version)			    //if file of correct version
            {
                for(int i=0;i<programs_num;i++)		    //load info to programs database
                {
                    QChar sym;
                    in>>sym;
                    if(sym=='1')
                        programs[i].checked=true;
                    if(sym=='0')
                        programs[i].checked=false;
                }
                inF.close();				    //close file
                checkboxes_recheck();			    //refresh checkboxes
                if(language_cur==1)
                    QMessageBox::information(this, "Выполнено!", "Отметки загружены успешно.");
                if(language_cur==2)
                    QMessageBox::information(this, "Виконано!", "Позначки вдало завантажені.");
                if(language_cur==3)
                    QMessageBox::information(this, "Success!", "Selections loaded successfully.");
            }
            else
            {						    //if file of not correct version
                if(language_cur==1)
                    QMessageBox::information(this, "Ошибка", "Версия файла не совпадает с версией FP SoftPack'а.");
                if(language_cur==2)
                    QMessageBox::information(this, "Помилка", "Версія файлу не співпадає з версією FP SoftPack'у.");
                if(language_cur==3)
                    QMessageBox::information(this, "Error", "File version not match to the FP SoftPack version.");
            }
            inF.close();
        }
        else
        {							    //if file don't opened correctly
            if(language_cur==1)
                QMessageBox::information(this, "Ошибка", "Файл "+path+" не удалось открыть для чтения.");
            if(language_cur==2)
                QMessageBox::information(this, "Помилка", "Файл "+path+" не вдалося відкрити для читання.");
            if(language_cur==3)
                QMessageBox::information(this, "Error", "Failed to open file "+path+" for reading.");
        }
    }
}

void MainWindow::keyboard_update_index()
{
    if(keyboard_mode==false)
    {
        keyboard_mode=true;
        keyboard_cur_index=0;
    }
    int check_num=0;
    for(int i=0;i<checkboxes_num;i++)
        if(pages[page_cur][i]!=0)
            check_num++;
    if(keyboard_cur_index<0)
        keyboard_cur_index=check_num-1;
    if(keyboard_cur_index>=check_num)
        keyboard_cur_index=0;
    for(int i=0;i<checkboxes_num;i++)
    {
        bool under=false;
        if(i==keyboard_cur_index)
            under=true;
        else
            under=false;
        bool bold=false;
        if(pages[page_cur][i]>0)
            bold=false;
        else
            bold=true;
        setFont(&chbox[i], checkbox_font_family, checkbox_font_size, bold, 0, under);
    }
}


void MainWindow::move(QLabel* aim, int Xend, int Yend, int t)
{
    int db_num=getObjNum(aim);//number of object
    bool ipair=false;           //if we have pair
    QPushButton* pair;           //adress of pair
    if(db_num!=-1)              //check if we have pair
        ipair=true;
    if(ipair)                   //get adress
        pair=par[db_num].pair;
    int nk=t/32;
    double Xcur=aim->x();
    double Ycur=aim->y();
    double VX=0;
    double VY=0;
    double aX=1*(Xend-Xcur)/(nk*nk);
    double aY=1*(Yend-Ycur)/(nk*nk);
    for(int i=0;i<nk;i++)
    {
        VX+=aX;
        VY+=aY;
        Xcur+=VX;
        Ycur+=VY;
        aim->move(Xcur, Ycur);
        if(ipair)
            pair->move(Xcur, Ycur);
        sleep(16);
    }
    for(int i=0;i<nk;i++)
    {
        VX-=aX;
        VY-=aY;
        Xcur+=VX;
        Ycur+=VY;
        aim->move(Xcur, Ycur);
        if(ipair)
            pair->move(Xcur, Ycur);
        sleep(16);
    }
    aim->move(Xend, Yend);
    if(ipair)
        pair->move(Xend, Yend);
}
