from re import match, sub
from typing import List

# def findPreviousLines(string : str, index : int, count : int) -> List[int]:
#     """
#         Returns list of indices of lines before given index (ignoring empty lines). 

#             Parameters:
#                 string (str): Input string
#                 index (int): Index in string from where to start
#                 count (int): Number of previous lines to return

#             Returns:
#                 indices (List[int]): List of indices of lines before given index
#     """ 
#     #Only need substring left of given index
#     substring = string[:index]

#     #get reversed iterator of all characters and their indices in string
#     reversed_iterator = reversed(list(enumerate(substring)))
#     #Iterate up to index of newline character that precedes given index
#     for _, character in reversed_iterator:
#         if character == '\n':
#             break

#     line_count = 0
#     #Find all indices of lines that precedge given index's line
#     indices = [i+1 if i != 0 else i
#         for i, character in reversed_iterator
#         if (character == '\n' and substring[i-1] != '\n' or i == 0)
#         and (line_count := line_count+1) <= count]
#     #Return list of indices in order of given string
#     return reversed(indices)

# def findFollowingLines(string : str, index : int, count : int) -> List[int]:
#     """
#         Returns list of indices of lines after given index of string (ignoring empty lines). 

#             Parameters:
#                 string (str): Input string
#                 index (int): Index in string from where to start
#                 count (int): Number of following lines to return

#             Returns:
#                 indices (List[int]): List of indices of lines after given index
#     """ 
#     #Only need substring right of given index
#     substring = string[index+1:]

#     #get iterator of all characters and their indices in substring
#     iterator = enumerate(substring)

#     line_count = 0
#     #Find all indices of lines that follow given index's line
#     indices = [i+index+1
#         for i, character in iterator
#         if character == '\n' and substring[i+1] != '\n' and (line_count := line_count+1) <= count]
#     #Return list of indices
#     return indices

def findPreviousLines(string : str, index : int, count : int) -> str:
    """
        Returns string of lines before given index (ignoring empty lines). 

            Parameters:
                string (str): Input string
                index (int): Index in string from where to start
                count (int): Number of previous lines to return

            Returns:
                lines str: String of lines before given index
    """ 
    if count <= 0:
        return ""

    #Only need substring left of given index and including given index
    substring = string[:index+1]

    # Do not need line that includes given index
    lines = [line for line in substring.splitlines()[:-1] if line != '']

    #Return only amount of lines requested
    if count >= len(lines):
        result = "\n".join(lines)
        return result
    if count <= len(lines):
        result = "\n".join(lines[-(count):])
        return result

def findFollowingLines(string : str, index : int, count : int) -> str:
    """
        Returns string of lines after given index of string (ignoring empty lines). 

            Parameters:
                string (str): Input string
                index (int): Index in string from where to start
                count (int): Number of following lines to return

            Returns:
                lines (str): String of lines after given index
    """ 
    if count <= 0:
        return ""

    #Only need substring left of given index and including given index
    substring = string[index:]

    # Do not need line that includes given index
    lines = [line for line in substring.splitlines()[1:] if line != '']

    #Return only amount of lines requested
    if count >= len(lines):
        return "\n".join(lines)
    if count <= len(lines):
        return "\n".join(lines[:count])

def findCurrentLine(string : str, index : int) -> str:
    """
        Returns string of line that contains given index. 

            Parameters:
                string (str): Input string
                index (int): Index in string from where to start

            Returns:
                line (str): String of line that contains given index
    """
    #Find new line to left of index
    line_start = string.rfind('\n', 0, index)
    line_start += 1
    #Find new line to right of index
    line_end = string.find('\n', index)
    if line_end == -1:
        line_end = len(string)

    result = string[line_start:line_end]
    return result

if __name__ == '__main__':
    string = "test\nokay\nyes\nno\ngreat"

    #print(findFollowingLines(string, 10, 2))
    print(findCurrentLine(string, 10))