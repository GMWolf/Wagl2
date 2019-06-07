if(GLAD_DEBUG)
    set(glad_generator "c-debug")
    set (glad_dir ${CMAKE_CURRENT_LIST_DIR}/c-debug)
else()
    set(glad_generator "c")
    set (glad_dir ${CMAKE_CURRENT_LIST_DIR}/c)
endif()

set(glad_arg "--out-path ${glad_dir} --profile=\"core\" --api \"gl=4.5\" --generator ${glad_generator} --spec \"gl\" --extensions extensions.txt")

message(${glad_arg})

set (glad_output
        ${glad_dir}/include/glad/glad.h
        ${glad_dir}/include/KHR/khrplatform.h
        ${glad_dir}/src/glad.c
        )

add_custom_command(
        COMMAND glad ${glad_arg}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT ${glad_output}
        MAIN_DEPENDENCY extensions.txt
        COMMENT "Generating glad"
)

add_library(glad ${glad_output})
target_include_directories(glad PUBLIC ${glad_dir}/include)