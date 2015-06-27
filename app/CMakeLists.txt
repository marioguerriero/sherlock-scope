set(APP_DIR "/app")

# Install files
install(DIRECTORY "data" DESTINATION ${APP_DIR})
install(FILES "sherlock.apparmor" DESTINATION ${APP_DIR})
install(FILES "sherlock.content-hub" DESTINATION ${APP_DIR})

# Add subdirectories
add_subdirectory(qml)
#add_subdirectory(po) Disable for now

add_custom_target("autopilot" chmod +x ${CMAKE_SOURCE_DIR}/qml/tests/autopilot/run
                    COMMAND ${CMAKE_SOURCE_DIR}/qml/tests/autopilot/run
                    WORKING_DIRECTORY ./qml)

add_custom_target("check" /usr/bin/qmltestrunner -input ${CMAKE_SOURCE_DIR}/qml/tests/unit -import ${CMAKE_BINARY_DIR}/backend
                    WORKING_DIRECTORY ./qml)

add_custom_target("run" /usr/bin/qmlscene -I ${CMAKE_BINARY_DIR}/backend  ${CMAKE_SOURCE_DIR}/qml/sherlock.qml
                    WORKING_DIRECTORY ./qml)

# Normally QtCreator would only show files that are part of a target, but we need it to show also files 
# that are not compiled. Therefore we add a custom target that just does nothing but list the files 
add_custom_target("sherlock_ClickFiles" ALL SOURCES "sherlock.apparmor")
