#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <qt\qhttp.h>
#define objects_num 100		    //numder of objects can be saved in the variables
#define max_commands_num 100	    //max number of synchronized commands
#define timer_rate 40		    //animation timer delay
#define parameters_max_num 100      //max num of saved parameters in parameters file
#define themes_max_num 30           //max num of used themes
#define programs_max_num 500	    //max number of programs can be loaded
#define categories_max_num 20	    //max number of categories may be parsed
#define pages_max_num 10            //max number of generated pages
#define checkboxes_max_num 100      //max number of generated checkboxes
#define installing_button_num 5     //max number of buttons on program page
#define quere_items_max_num 6       //max number of shown itens of quere

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    struct p{			    //database objects which need to be animated
	    bool exis;			//is object exist
	    bool anim;			//animate as button?
	    QLabel *obj;		//object adress
	    QPushButton *pair;		//pair object(as button and background)
	    int opac;			//opacity at start
	    int opac_hovered;		//if need to animate, opacity whem object hovered
	    int opac_normal;		//when not hovered
	    QString img;		//imagefile
	    QPixmap img_buffer;		//buffer image
	    QString img_buffer_filename;//name of buffered file
    }par[objects_num];
    int getObjNum(QLabel*);             //get object number from the batabase by adress
    void apply_graphics();		//reload all pixmaps to all labels
    void on_show();                     //starts immediately after window shows
    void state_update(int);             //reload all objects according to current state(1=soft list 2=program page)
    void setColor(QWidget*, QString, QString, QString);//sets color to received widget(widget, normal color, hovered, clicked)
    void setFont(QWidget*,QString,int,bool,bool,bool);//set font to received objects(aim widget,font name, size, bold, italic, underlined)
    int state_current;			//1=list page; 2=program page
    bool alr1;				//boolean variable which indicates first run of code on 1st state(false=first)
    bool alr2;				//boolean variable which indicates first run of code on 2nd state(false=first)

    //------buttons-----
    int button_font_size;		//size of button text
    QString button_font_name;		//name of button font
    bool button_font_bold;		//if bold
    bool button_font_italic;		//if italic
    bool button_font_underlined;	//if underlined
    void button_visible(QLabel* label, bool visible); //set visible

    //------folders-----
    QString folder_themes;              //folder with themes
    QString folder_graphics;            //folder with images
    QString folder_windows;             //folder when windows installed
    QString folder_desktop;             //folder desktop
    QString folder_appdata;		//folder when we can store some information
    QString folder_audio;               //folder when located all audio
    QString file_parameters;            //file to save parameters
    QString file_version;		//file when directed version of current softpack
    QString file_help_ua;                  //help fole adress
    QString file_help_ru;                  //help fole adress
    QString file_help_en;                  //help fole adress

    //------themes-----
    QString themes[themes_max_num];     //names and numbers of registered themes
    QString theme_basic;                //basic theme whitch aplyes whin softpack starts first time
    int theme_cur;                      //current theme id
    int themes_num;                     //number of detected themes
    QString theme_color_buttons_normal;	//color (loads from file in theme)
    QString theme_color_buttons_hover;	//color (loads from file in theme)
    QString theme_color_buttons_pressed;//color (loads from file in theme)
    QString theme_color_nextback_normal;	//color (loads from file in theme)
    QString theme_color_nextback_hover;	//color (loads from file in theme)
    QString theme_color_nextback_pressed;//color (loads from file in theme)
    QString theme_color_header;         //color (loads from file in theme)
    QString theme_color_help_text;      //color (loads from file in theme)
    QString theme_color_pages;          //color (loads from file in theme)
    QString theme_color_headbuttons_normal;//color (loads from file in theme)
    QString theme_color_headbuttons_hover;//color (loads from file in theme)
    QString theme_color_headbuttons_pressed;//color (loads from file in theme)
    QString theme_color_copyright;      //color (loads from file in theme)
    QString theme_color_checkboxes_normal;//color (loads from file in theme)
    QString theme_color_checkboxes_hover;//color (loads from file in theme)
    QString theme_color_checkboxes_pressed;//color (loads from file in theme)
    QString theme_color_radiobutton_normal;//color (loads from file in theme)
    QString theme_color_radiobutton_hover;//color (loads from file in theme)
    QString theme_color_radiobutton_pressed;//color (loads from file in theme)
    QString theme_file_background;	//name of file directed in theme folder
    QString theme_file_button;		//name of file directed in theme folder
    QString theme_file_nextback;	//name of file directed in theme folder
    QString theme_file_colors;		//name of file directed in theme folder
    int theme_file_colors_lines_num;	//number of lines in colors.ini
    QString *theme_file_colors_lines_aim[40];//which line fron file must be loaded to which variable

    //------checkboxes-----
    QCheckBox chbox[checkboxes_max_num];//checkboxes
    int checkboxes_top;                 //maximum top position to checkboxes
    int checkboxes_bottom;              //maximum bottom position to checkboxes
    int checkboxes_left;                //maximum left position to checkboxes
    int checkboxes_right;               //maximum right position to checkboxes
    int checkbox_size_x;                //x size of one checkbox (width)
    int checkbox_size_y;                //y size of one checkbox (height)
    int checkbox_interval_x;            //x interval between checkboxes
    int checkbox_interval_y;            //y interval between checkboxes
    int checkboxes_num;                 //num of checkboxes
    int checkbox_font_size;             //size of text
    QString checkbox_font_family;	//font family of text
    void checkboxes_recheck();		//recheck only program checkboxes

    //------black-----
    int black_max;			//maximum black value
    QLabel *black_label;		//black label
    QString black_imagefile;		//screensaver image
    int black_on_speed;			//speed turning on black
    int black_off_speed;		//speed turning off black

    //------window-----
    QString window_title;		//title of window
    QString version;			//current version loaded from v-file
    int window_size_X;
    int window_size_Y;

    //------parameters-----
    bool parameter_exist(QString);      //check if parameter exists
    QString parameter_get(QString);     //get parameter value to QString
    void parameter_set(QString, QString);//sets parameter(name,data)

    //------animation-----
    struct command{		    //List of highlighting buttons
	bool exist;			//is command not empty
	QString type;			//opacity, slow-change
	QLabel *object;			//adress of object
	int aim;			//needed opacity value
	int step;			//step to plus/minus
    } commands[max_commands_num];
    QPixmap opacity(QPixmap,int);	//processes pixmap to set opacity
    bool last_hover[objects_num];	//controls hover and leave actions
    QTimer *animation;			//timer to work animation
    void addCommand(QLabel*,int,int);   //add button opacity command(type,object,aim,step,argument)
    //----------tooltips--------
    int tooltips_timer_delay;		//timer for interval
    QTimer *tooltips_timer;		//timer
    int tooltips_last_hover[objects_num];//controls mouse position and time to show tooltips
    int tooltip_delay;			//time which need to show message
    struct tt{
	bool exist;		//if tooltip exist
	QWidget *object;	//link to object
	QString text;		//tooltip text
    }tooltips[objects_num];		//database when saved tooltip texts
    void tooltip_set(QWidget*,bool, QString);//tets tooltis (object,tooltip on\off,text)
    //----------categories--------
    QStringList categories;	//all names of categories
    int categories_num;		//number of categories
    bool category_checked(int);
    //----------programs--------
    struct db{
        QString name;			//program name
        //QString description;		//program description
        QString description_ru;		//program description
        QString description_ua;		//program description
        QString description_en;		//program description
        int category;                   //number of category in categories
        QStringList files;              //files of this program
        bool checked;                   //true if must be installed
    }programs[programs_max_num];		//programs database
    int programs_num;				//num of found folders
    int program_cur;                            //number of program (page) currently viewing
    QString programs_folder;                    //folder with soft
    QString programs_file_screenshot;           //name of screenshot file
    QString programs_file_installer;            //part of name installer file
    QString programs_file_description;          //name of description file basic
    //----------pages--------
    int pages[pages_max_num][checkboxes_max_num];//pages consists numbers of programs(+) and groups(-)
    int pages_num;                               //numer of used pages
    int page_cur;                                //number of page currently viewing
    void pages_clip();				 //uses to anumate different things
    //----------installing--------
    int installing_selected[programs_max_num];	//list of selected checkboxes
    int installing_selected_num;		//number of selected checkboxes
    int installing_cur;				//current program installing
    QLabel installing_screenshot_button_back;	//screenshot image
    QPushButton installing_screenshot_button;	//screenshot button
    int installing_screenshot_button_size_x;	//screenshot width
    int installing_screenshot_button_size_y;	//screenshot height
    QString installing_screenshot_file_default;	//default image which shows when real image doesn't exist
    QLabel installing_button_back[installing_button_num];//buttons on program page back
    QPushButton installing_button[installing_button_num];//buttons on programs page
    int installing_button_size_x;		//buttons on program page width
    int installing_button_size_y;		//buttons on program page heigth
    int installing_button_interval_y;		//buttons on program page interval
    QLabel installing_button_open_back;
    QPushButton installing_button_open;
    int installing_button_open_x;
    int installing_button_open_y;
    int installing_button_open_size_x;
    int installing_button_open_size_y;
    QLabel installing_description;		//program description
    int installing_description_font_size;	//program description font size
    QString installing_description_font_name;	//program description font name
    int installing_description_size_x;		//program description width
    int installing_description_size_y;		//program description heigth
    QLabel installing_name;			//program name label
    int installing_name_size_y;			//program name height
    int installing_name_font_size;		//program name font size
    QString installing_name_font_name;		//program name font name
    int installing_button_last_right_click_number;//number of last right clicked button
    int installing_button_menu_size_x;          //size of menu showed when clicked button
    int installing_button_menu_size_y;          //size of menu showed when clicked button
    QListWidget *installing_button_menu_list;   //menu showed when clicked button
    int installing_button_text_length;          //maximum text lenght before cutting

    //----------search--------
    QString search_last;		//last user's reqest for searching
    int search_pages_tmp[pages_max_num][checkboxes_max_num];//variable to save backup of original pages
    int search_pages_num_tmp;		//viriable to save backup of origina pages number
    int search_found_num;		//number of found programs
    bool search_mode;			//true if now if finding sonething
    QString search_button_noral_mode;	//filename of image which loads to search button when serach mode off
    QString search_button_serch_mode;	//filename of image which loads to search button when serach mode on
    int search_found[programs_max_num];  //massive when stored all found programs
    void search(QString);
    //----------save--------
    QString save_file_extension;	//extension of save file
    QString save_directory;		//default directory to save files
    int save_menu_size_x;               //size of menu showed when clicked button
    int save_menu_size_y;               //size of menu showed when clicked button
    QListWidget *save_list;             //menu showed when clicked button
    void save();
    void open();
    //----------quere--------
    struct qe{
        QString file;                   //to save name of file
        QString name;                   //to save name of program
	QString category;               //to save name of category
    }quere[quere_items_max_num];        //to store quere all items
    int quere_items_num;                //nuber of item s saved in quere variable
    QLabel quere_back_label[quere_items_max_num];//label fo image
    QString quere_back_file;		//filename loaded to background  label
    QPushButton quere_button[quere_items_max_num];//buttons for quere item
    int quere_item_size_x;		//size of ihem
    int quere_item_size_y;		//size of ihem
    int quere_item_interval_y;		//interval between ihems
    int quere_position_x;		//position of quere
    int quere_position_y;		//position of quere
    QListWidget *quere_menu_list;       //menu showed when clicked button
    int quere_menu_size_x;		//size of menu shown when item right-clicked
    int quere_menu_size_y;		//size of menu shown when item right-clicked
    void quere_menu_show();		//function shows menu of quere item
    int quere_last_button_clicked;	//variable when stored number of last clicked item of quere to identify aim in function
    void quere_update();		//refresh quere
    //----------audio--------
    QString audio_button_hover_sound_file;
    QString audio_button_click_sound_file;
    //----------update--------
    QString update_download_host;
    QString update_download_file;
    QString update_download_to;
    QHttp update_http;
    QFile update_file;
    QString update_ver_exception;
    QString update_text;
    QString update_site;
    //----------language--------
    int language_cur;                   //1=russian     2=ukrainian     3=english
    //----------keyboard--------
    int keyboard_cur_index;
    int keyboard_mode;
    void keyboard_update_index();
    bool keyboard_ctrl;                 //if ctrl pressed now
    bool keyboard_ignore_enter;         //when entered in input field
    //-----------moving---------
    int moving_A;
    void move(QLabel* aim, int x, int y, int t);

protected:
    virtual void mouseMoveEvent(QMouseEvent *m);//when move mouse
    void mouseReleaseEvent(QMouseEvent *event);//whem mouse button released
    void closeEvent(QCloseEvent *event);	//when close window
    void keyPressEvent(QKeyEvent *event);       //when key pressed
    void keyReleaseEvent(QKeyEvent *event);       //when key released
private:
    Ui::MainWindow *ui;
private slots:
    void onTimer();			//button animation
    void tooltips_on_timer();		//works on timer (don't forget about QCursor::pos() !!!)
    void apply_theme(int);		//apply the theme(in variables) by number
    void category_check();		//check all the members of category
    void onThemeBox_changed(int);       //when combobox changed
    void save_on_selected_item(QModelIndex);	//on clicking save menu
    //buttons
    void on_button_1_clicked();		//on clicking button
    void on_button_2_clicked();		//on clicking button
    void on_button_3_clicked();		//on clicking button
    void on_button_4_clicked();		//on clicking button
    void on_button_back_clicked();	//on clicking button
    void on_button_next_clicked();	//on clicking button
    void on_pages_number_edit_returnPressed();//text input for manual navigation
    void on_button_save_clicked();	//for save checkes
    void on_button_search_clicked();	//for search program
    void on_button_about_clicked();
    void on_button_help_clicked();
    //installing
    void installing_button_1_clicked(); //clicking button on installing page
    void installing_button_2_clicked(); //clicking button on installing page
    void installing_button_3_clicked(); //clicking button on installing page
    void installing_button_4_clicked(); //clicking button on installing page
    void installing_button_5_clicked(); //clicking button on installing page
    void installing_button_open_clicked(); //clicking button for open folder
    void installing_button_screenshot_clicked(); //clicking screenshot on installing page
    void installing_button_menu_on_selected_item(QModelIndex); //clicking item of ionstallingbutton context menu page
    //quere
    void quere_item_1_clicked();	//when quere item clicked
    void quere_item_2_clicked();	//when quere item clicked
    void quere_item_3_clicked();	//when quere item clicked
    void quere_item_4_clicked();	//when quere item clicked
    void quere_item_5_clicked();	//when quere item clicked
    void quere_item_6_clicked();	//when quere item clicked
    void quere_menu_selected(QModelIndex);//when one of the items of quere right-click menu selected
    //update
    void update_done(bool);
    void update_read_progress(int,int);
    void on_button_update_clicked();
    void onLanguageBox_changed(int);       //when combobox changed
};

#endif // MAINWINDOW_H
