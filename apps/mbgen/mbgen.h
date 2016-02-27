/*++

Copyright (c) 2015 Minoca Corp. All Rights Reserved

Module Name:

    mbgen.h

Abstract:

    This header contains definitions for the Minoca Build Generator.

Author:

    Evan Green 3-Dec-2015

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <minoca/lib/types.h>
#include <minoca/lib/status.h>
#include "chalk.h"

//
// --------------------------------------------------------------------- Macros
//

#define MBGEN_IS_SPECIAL_PATH_CHARACTER(_Character) \
    (((_Character) == '/') || ((_Character) == '\\') || ((_Character) == '^'))

#define MBGEN_IS_SOURCE_ROOT_RELATIVE(_String) \
    (((_String)[0] == '/') && ((_String)[1] == '/'))

#define MBGEN_IS_BUILD_ROOT_RELATIVE(_String) \
    (((_String)[0] == '^') && ((_String)[1] == '/'))

//
// ---------------------------------------------------------------- Definitions
//

#define MBGEN_PROJECT_FILE ".mbproj"
#define MBGEN_BUILD_FILE "build.mb"
#define MBGEN_DEFAULT_NAME "//:"

#define MBGEN_OPTION_VERBOSE 0x00000001
#define MBGEN_OPTION_DEBUG 0x00000002
#define MBGEN_OPTION_DRY_RUN 0x00000004

#define MBGEN_TARGET_PHONY 0x00000001

//
// ------------------------------------------------------ Data Type Definitions
//

typedef enum _MBGEN_DIRECTORY_TREE {
    MbgenDirectoryTreeInvalid,
    MbgenSourceTree,
    MbgenBuildTree,
    MbgenAbsolutePath,
} MBGEN_DIRECTORY_TREE, *PMBGEN_DIRECTORY_TREE;

typedef enum _MBGEN_INPUT_TYPE {
    MbgenInputInvalid,
    MbgenInputSource,
    MbgenInputTarget,
} MBGEN_INPUT_TYPE, *PMBGEN_INPUT_TYPE;

typedef enum _MBGEN_SCRIPT_ORDER {
    MbgenScriptOrderInvalid,
    MbgenScriptOrderCommandLine,
    MbgenScriptOrderProjectRoot,
    MbgenScriptOrderGlobal,
    MbgenScriptOrderTarget,
} MBGEN_SCRIPT_ORDER, *PMBGEN_SCRIPT_ORDER;

typedef struct _MBGEN_TARGET MBGEN_TARGET, *PMBGEN_TARGET;

/*++

Structure Description:

    This structure stores the global information for an instantiation of the
    Minoca Build Generator application.

Members:

    Options - Stores the bitfield of application options. see MBGEN_OPTION_*
        definitions.

    Interpreter - Stores the Chalk interpreter context.

    ProjectFileName - Stores a pointer to the project file name (just the name,
        not the directory).

    BuildFileName - Stores a pointer ot the build file name to look for in each
        directory.

    SourceRoot - Stores a pointer to the source root directory.

    BuildRoot - Stores a pointer to the build root directory.

    ScriptList - Stores the head of the list of loaded scripts.

    GlobalName - Stores a pointer to the name of the global environment file
        to load into all targets.

    DefaultName - Stores a pointer to the name of the default target. If not
        specified, defaults to //:.

    ToolList - Stores a pointer to a list of tools defined. Tools are global
        across the build.

--*/

typedef struct _MBGEN_CONTEXT {
    ULONG Options;
    CHALK_INTERPRETER Interpreter;
    PSTR ProjectFileName;
    PSTR BuildFileName;
    PSTR SourceRoot;
    PSTR BuildRoot;
    LIST_ENTRY ScriptList;
    PSTR GlobalName;
    PSTR DefaultName;
    LIST_ENTRY ToolList;
} MBGEN_CONTEXT, *PMBGEN_CONTEXT;

/*++

Structure Description:

    This structure stores the components of a fully specified build target path.

Members:

    Root - Stores the directory tree root for the target.

    Path - Stores the directory path relative to the root of the target.

    Target - Stores the target name.

--*/

typedef struct _MBGEN_PATH {
    MBGEN_DIRECTORY_TREE Root;
    PSTR Path;
    PSTR Target;
} MBGEN_PATH, *PMBGEN_PATH;

/*++

Structure Description:

    This structure stores an array of inputs, which are either of type source
    or target.

Members:

    Array - Stores a pointer to the array of pointers to elements. Each element
        is of type MBGEN_SOURCE or MBGEN_TARGET depending on their first field
        value.

    Count - Stores the number of elements in the array.

    Capacity - Stores the maximum number of elements in the array before the
        array must be resized.

--*/

typedef struct _MBGEN_INPUTS {
    PVOID *Array;
    ULONG Count;
    ULONG Capacity;
} MBGEN_INPUTS, *PMBGEN_INPUTS;

/*++

Structure Description:

    This structure stores a tool definition.

Members:

    ListEntry - Stores pointers to the next and previous tools in the build.

    Name - Stores a pointer to the name of the tool. This must be unique across
        the build.

    Command - Stores a pointer to the command to run to execute the tool.

    Description - Stores a pointer to the description to print when executing
        the tool.

    Depfile - Stores a pointer to the dependency file output name.

    DepsFormat - Stores a pointer to the dependency file format: MSVC or GCC.

--*/

typedef struct _MBGEN_TOOL {
    LIST_ENTRY ListEntry;
    PSTR Name;
    PSTR Command;
    PSTR Description;
    PSTR Depfile;
    PSTR DepsFormat;
} MBGEN_TOOL, *PMBGEN_TOOL;

/*++

Structure Description:

    This structure stores information about a loaded script.

Members:

    ListEntry - Stores pointers to the next and previous scripts in the build.

    Root - Stores the directory tree root for the target.

    Path - Stores the directory path relative to the root, not including the
        actual file name.

    CompletePath - Stores the complete file path to the script.

    Script - Stores a pointer to the script contents.

    Size - Stores the size of the script file in bytes, not including the
        artificially appended null terminator.

    Result - Stores the result returned from executing the script.

    TargetList - Stores the head of the list of targets in this script.

    TargetCount - Stores the number of targets on the target list.

--*/

typedef struct _MBGEN_SCRIPT {
    LIST_ENTRY ListEntry;
    MBGEN_DIRECTORY_TREE Root;
    PSTR Path;
    PSTR CompletePath;
    PSTR Script;
    UINTN Size;
    PCHALK_OBJECT Result;
    LIST_ENTRY TargetList;
    ULONG TargetCount;
} MBGEN_SCRIPT, *PMBGEN_SCRIPT;

/*++

Structure Description:

    This structure stores information about a loaded script.

Members:

    ListEntry - Stores pointers to the next and previous scripts in the build.

    Root - Stores the directory tree root for the target.

    Path - Stores the directory path relative to the root, not including the
        actual file name.

    CompletePath - Stores the complete file path to the script.

    Script - Stores a pointer to the script contents.

    Size - Stores the size of the script file in bytes, not including the
        artificially appended null terminator.

    Result - Stores the result returned from executing the script.

    TargetList - Stores the head of the list of targets in this script.

    TargetCount - Stores the number of targets on the target list.

--*/

typedef struct _MBGEN_SOURCE {
    MBGEN_INPUT_TYPE Type;
    MBGEN_DIRECTORY_TREE Tree;
    PSTR Path;
} MBGEN_SOURCE, *PMBGEN_SOURCE;

/*++

Structure Description:

    This structure stores a target definition.

Members:

    Type - Stores the type, which is always set to "target" since this is a
        target.

    ListEntry - Stores pointers to the next and previous tools in the build.

    Script - Stores a pointer back to the script that owns this target.

    Label - Stores a pointer to the name of the target.

    Output - Stores a pointer to the output name.

    Tree - Stores the tree the output path is rooted to.

    Tool - Stores the name of the tool used to build this target.

    Flags - Stores a bitfield of flags regarding the target. See MBGEN_TARGET_*
        definitions.

    Inputs - Stores the inputs to the target.

    OrderOnlyInputs - Stores the order-only inputs to the target.

    InputsObject - Stores a pointer to the list object of input strings.

    OrderOnlyInputsObject - Stores a pointer to the list of order-only input
        strings.

    Callback - Stores a pointer to the function to call back when this target
        is added as an input (but not an order-only input).

    Config - Stores a pointer to the configuration information for this target.

--*/

struct _MBGEN_TARGET {
    MBGEN_INPUT_TYPE Type;
    LIST_ENTRY ListEntry;
    PMBGEN_SCRIPT Script;
    PSTR Label;
    PSTR Output;
    MBGEN_DIRECTORY_TREE Tree;
    PSTR Tool;
    ULONG Flags;
    MBGEN_INPUTS Inputs;
    MBGEN_INPUTS OrderOnlyInputs;
    PCHALK_OBJECT InputsObject;
    PCHALK_OBJECT OrderOnlyInputsObject;
    PCHALK_OBJECT Callback;
    PCHALK_OBJECT Config;
};

//
// -------------------------------------------------------------------- Globals
//

//
// -------------------------------------------------------- Function Prototypes
//

//
// Main application functions
//

INT
MbgenParseScriptResults (
    PMBGEN_CONTEXT Context,
    PMBGEN_SCRIPT Script
    );

/*++

Routine Description:

    This routine parses the return value of a target script.

Arguments:

    Context - Supplies a pointer to the context.

    Script - Supplies a pointer to the script that just finished executing.

Return Value:

    0 on success.

    Returns an error number on failure.

--*/

VOID
MbgenDestroyTarget (
    PMBGEN_TARGET Target
    );

/*++

Routine Description:

    This routine destroys a target entry.

Arguments:

    Target - Supplies a pointer to the target to destroy.

Return Value:

    None.

--*/

//
// Script utility functions
//

INT
MbgenLoadTargetScript (
    PMBGEN_CONTEXT Context,
    PMBGEN_PATH Target,
    MBGEN_SCRIPT_ORDER Order,
    PMBGEN_SCRIPT *Script
    );

/*++

Routine Description:

    This routine loads the script corresponding to the given target specifier
    string.

Arguments:

    Context - Supplies a pointer to the application context.

    Target - Supplies a pointer to the target specifier to load.

    Order - Supplies the order to apply to the script.

    Script - Supplies a pointer where a pointer to the loaded or found script
        will be returned on success.

Return Value:

    0 on success.

    Non-zero on failure.

--*/

INT
MbgenLoadProjectRoot (
    PMBGEN_CONTEXT Context
    );

/*++

Routine Description:

    This routine loads and interprets the project root script.

Arguments:

    Context - Supplies a pointer to the application context.

Return Value:

    0 on success.

    Non-zero on failure.

--*/

INT
MbgenLoadScript (
    PMBGEN_CONTEXT Context,
    MBGEN_SCRIPT_ORDER Order,
    PMBGEN_PATH TargetPath,
    PMBGEN_SCRIPT *FinalScript
    );

/*++

Routine Description:

    This routine loads and interprets a given target path. If the script
    containing the given target path is already loaded, then this is a no-op.

Arguments:

    Context - Supplies a pointer to the application context.

    Order - Supplies the order to apply to the script.

    TargetPath - Supplies a pointer to the target path to load. The target
        name is ignored, only the root and path are observed.

    FinalScript - Supplies a pointer where a pointer to the newly loaded or
        found script will be returned on success.

Return Value:

    0 on success.

    Non-zero on failure.

--*/

VOID
MbgenDestroyAllScripts (
    PMBGEN_CONTEXT Context
    );

/*++

Routine Description:

    This routine destroys all scripts in the application context.

Arguments:

    Context - Supplies a pointer to the application context.

Return Value:

    0 on success.

    Non-zero on failure.

--*/

//
// Path utility functions
//

INT
MbgenParsePath (
    PMBGEN_CONTEXT Context,
    PSTR Name,
    MBGEN_DIRECTORY_TREE RelativeTree,
    PSTR RelativePath,
    PMBGEN_PATH Target
    );

/*++

Routine Description:

    This routine breaks an mbgen path string into its components.

Arguments:

    Context - Supplies a pointer to the application context.

    Name - Supplies a pointer to the name string.

    RelativeTree - Supplies the tree type (usually source or build) that
        relative paths are rooted against.

    RelativePath - Supplies a pointer to the path to prepend to relative paths.

    Target - Supplies a pointer where the target will be returned on success.
        The caller will be responsible for freeing the string buffers.

Return Value:

    0 on success.

    Returns an error number on failure.

--*/

PSTR
MbgenAppendPaths3 (
    PSTR Path1,
    PSTR Path2,
    PSTR Path3
    );

/*++

Routine Description:

    This routine appends three paths to one another.

Arguments:

    Path1 - Supplies a pointer to the first path.

    Path2 - Supplies a pointer to the second path.

    Path3 - Supplies a pointer to the second path.

Return Value:

    Returns a pointer to a newly created combined path on success. The caller
    is responsible for freeing this new path.

    NULL on allocation failure.

--*/

PSTR
MbgenAppendPaths (
    PSTR Path1,
    PSTR Path2
    );

/*++

Routine Description:

    This routine appends two paths to one another.

Arguments:

    Path1 - Supplies a pointer to the first path.

    Path2 - Supplies a pointer to the second path.

Return Value:

    Returns a pointer to a newly created combined path on success. The caller
    is responsible for freeing this new path.

    NULL on allocation failure.

--*/

INT
MbgenSetupRootDirectories (
    PMBGEN_CONTEXT Context
    );

/*++

Routine Description:

    This routine finds or validates the source root directory, and validates
    the build directory.

Arguments:

    Context - Supplies a pointer to the context.

Return Value:

    0 on success.

    Non-zero on failure.

--*/

PSTR
MbgenPathForTree (
    PMBGEN_CONTEXT Context,
    MBGEN_DIRECTORY_TREE Tree
    );

/*++

Routine Description:

    This routine returns the path for the given tree root.

Arguments:

    Context - Supplies a pointer to the context.

    Tree - Supplies the root to return.

Return Value:

    Returns the path of the given tree. The caller does not own this memory.

--*/

PSTR
MbgenSplitExtension (
    PSTR Path,
    PSTR *Extension
    );

/*++

Routine Description:

    This routine splits the extension portion off the end of a file path.

Arguments:

    Path - Supplies a pointer to the path to split.

    Extension - Supplies a pointer where a pointer to the extension will be
        returned on success. This memory will be part of the return value
        allocation, and does not need to be explicitly freed. This returns NULL
        if the path contains no extension or is a directory (ends in a slash).

Return Value:

    Returns a copy of the string, chopped before the last period. It is the
    caller's responsibility to free this memory.

    NULL on allocation failure.

--*/

//
// Chalk support functions
//

INT
MbgenAddChalkBuiltins (
    PMBGEN_CONTEXT Context
    );

/*++

Routine Description:

    This routine adds the functions in the global scope of the Chalk
    interpreter for the mbgen program.

Arguments:

    Context - Supplies a pointer to the application context.

Return Value:

    0 on success.

    Non-zero on failure.

--*/

//
// Output generator functions
//

INT
MbgenCreateMakefile (
    PMBGEN_CONTEXT Context
    );

/*++

Routine Description:

    This routine creates a Makefile out of the build graph.

Arguments:

    Context - Supplies a pointer to the application context.

Return Value:

    0 on success.

    Returns an error number on failure.

--*/

