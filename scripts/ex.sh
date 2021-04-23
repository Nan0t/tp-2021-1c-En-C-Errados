#!/bin/bash

declare BIN_DIR="../outputs/bin"
declare PROJECT=""
declare WKS_DIR=$( pwd )
declare CONFIG="Debug"
declare OPT=""
declare ARGS=""

declare SCRIPT_DIR="$( dirname $0 )"

function print_help {
    echo "Usage: ./ex <PROJECT-NAME> [OPTIONS] [ARGS]"
    echo ""
    echo "Options:"
    echo "--cfg             Target proyect configuration to execute. One of debug|release"
    echo "--debug-tool      Debuggin tool to attach. One of gdb|valgrind"
    echo "--args            Arguments of the executable"
    echo "--arch            Current target architecture"

    exit -1
}

function set_cfg {
    if [ "$1" == "debug" ]||[ "$1" == "release" ];then
        CONFIG=$1
    else
        echo "Invalid configuration."
        echo ""
        print_help
    fi

    shift;
}

function set_debug_tool {
    if [ "$1" == "gdb" ];then
        OPT="gdb --args"
    elif [ "$1" ==  "valgrind" ];then
        OPT="valgrind --leak-check=full --log-file=valgrind.log"
    elif [ "$1" == "helgrind" ];then
        OPT="valgrind --tool=helgrind"
    else
        echo "Invalid debug tool."
        echo ""
        print_help
    fi

    shift;
}

function set_args {
    ARGS="$ARGS $1"
}

declare -i SET_CFG_INDEX=0
declare -i SET_DEBUG_TOOL_INDEX=1
declare -i SET_ARGS_INDEX=2
declare -i SET_ARCH_INDEX=3

declare -a options=(set_cfg set_debug_tool set_args set_arch)

declare -i current_func=0

cd $SCRIPT_DIR

if [ $# -eq 0 ];then
    echo "Error. Not enought arguments."
    echo ""
    print_help
elif [ "$1" == "--help" ];then
    print_help
else
    PROJECT=$1
    shift;

    if [ $# -gt 0 ];then
        for value in $@
        do
            if [[ $value =~ --[a-z]+ ]];then
                if [ "$value" == "--cfg" ];then
                    current_func=SET_CFG_INDEX
                elif [ "$value" == "--debug-tool" ];then
                    current_func=SET_DEBUG_TOOL_INDEX
                elif [ "$value" == "--args" ];then
                    current_func=SET_ARGS_INDEX
                elif [ "$value" == "--arch" ];then
                    current_func=SET_ARCH_INDEX
                else
                    echo "Invalid option."
                    echo ""
                    print_help
                fi
                shift;
            else
                ${options[$current_func]} $1
                shift;
            fi
        done
    fi

    cd ../$PROJECT && $OPT $BIN_DIR/$PROJECT/$CONFIG/$PROJECT $ARGS
fi