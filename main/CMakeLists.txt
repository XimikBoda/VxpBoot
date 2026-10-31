set(TARGET_NAME "main")

set(SOURCES main.cpp boot.cpp
    UI/Profont6x11.h
    UI/UIBase.h
    UI/UIMenuBase.h UI/UIMenuBase.cpp
    UI/UIEngine.h UI/UIEngine.cpp
    UI/UITextBox.h UI/UITextBox.cpp
    UI/UIComboBox.h UI/UIComboBox.cpp
    UI/UIButton.h UI/UIButton.cpp
    UI/UICheckBox.h UI/UICheckBox.cpp
    UI/UIBootMenu.h UI/UIBootMenu.cpp
    UI/T2Input.h UI/T2Input.cpp
    UI/Render.h UI/Render.cpp
)

add_exec_vxp(${TARGET_NAME} 
    SRCS ${SOURCES}
    RESOURCES resources
    APP_NAME "${APP_NAME}"
    APPID "${APPID}"
    BACKGROUND ${BACKGROUND}
    API "${API}"
    RAM "${RAM}"
)

target_link_libraries(${TARGET_NAME} mreapi)