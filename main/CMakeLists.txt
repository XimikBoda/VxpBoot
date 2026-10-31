set(TARGET_NAME "main")

set(SOURCES main.cpp boot.cpp
    Profont6x11.h
    UIBase.h
    UIMenuBase.h UIMenuBase.cpp
    UIEngine.h UIEngine.cpp
    UITextBox.h UITextBox.cpp
    UIComboBox.h UIComboBox.cpp
    UIButton.h UIButton.cpp
    UIBootMenu.h UIBootMenu.cpp
    T2Input.h T2Input.cpp
    Render.h Render.cpp
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