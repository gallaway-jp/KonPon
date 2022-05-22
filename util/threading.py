from contextlib import suppress
from threading import Thread
from multiprocessing import Process, Manager, Array
from queue import Queue
from typing import List, Tuple

def work_in_background(func : object) -> None:
    """Adds simple background thread to process given function func.

            Parameters:
                func (object): Function to process in background thread

            Returns:
                N/A
    """     
    if not callable(func):
        return
    thread = Thread(target=func)
    thread.start()

def work_multiple_and_wait(funcs : List[object]) -> None:
    """Adds multiple background threads and waits until all finish

            Parameters:
                funcs (List[object]): Functions to process in background thread

            Returns:
                N/A
    """ 
    for func in funcs:
        if not callable(func):
            return
    threads : List[Thread] = []
    for func in funcs:
        threads.append(Thread(target=func))
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

def return_via_parameter(return_values : list, value_index : int, func : object, params : tuple) -> None:
    if not callable(func):
            return
    with suppress(TypeError):
        return_values[value_index] = func(*params)

def work_multiple_return_values(funcs : List[object], params : List[tuple]) -> list:
    """
        Adds multiple background threads and waits until all finish, returns values

            Parameters:
                funcs (List[object]): Functions to process in background process
                params (List[tuple]): List of parameters for each function. Each item in list is a tuple of parameters.

            Returns:
                return_values (list): List of return values from each function call
    """ 
    for func in funcs:
        if not callable(func):
            return
    threads : List[Thread] = []
    for func in funcs:
        threads.append(Thread(target=func))
    return_values = [None] * len(funcs)
    for index, func in enumerate(funcs):
        pars = return_values, index, func, params[index]
        threads.append(Thread(target=return_via_parameter, args=pars))
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    return return_values

def work_multiple_as_processes_return_values(funcs : List[object], params : List[tuple]) -> list:
    """Adds multiple background processes, waits until all finish, and returns values

            Parameters:
                funcs (List[object]): Functions to process in background thread
                params (List[tuple]): List of parameters for each function. Each item in list is a tuple of parameters.

            Returns:
                return_values (list): List of return values from each function call
    """ 
    for func in funcs:
        if not callable(func):
            return
    manager = Manager()
    return_values = manager.list()
    for _ in funcs:
        return_values.append(None)
    threads : List[Process] = []
    for index, func in enumerate(funcs):
        pars = return_values, index, func, params[index]
        threads.append(Process(target=return_via_parameter, args=pars))
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    return list(return_values)

def work_multiple_as_processes(funcs : List[object], params : List[tuple]) -> None:
    """Adds multiple background processes, waits until all finish

            Parameters:
                funcs List[object]: Functions to process in background thread

            Returns:
                N/A
    """ 
    for func in funcs:
        if not callable(func):
            return
    threads : List[Process] = []
    for index, func in enumerate(funcs):
        threads.append(Process(target=func, args=params[index]))
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

# class Workers():
#     def __init__(self) -> None:
#         self.threads : List[Thread] = [None] * 10
#         self.queue = Queue()
#         self._start_workers()
#     def _worker(self) -> None:
#         while True:
#             item : Tuple[object, tuple, int, list]
#             item = self.queue.get()
#             if item is None:
#                 break
#             (func, params, index, results) = item
#             results[index] = func(*params)
#             self.queue.task_done()
#     def _start_workers(self) -> None:
#         for i in range(len(self.threads)):
#             self.threads[i] = (Thread(target=self._worker))
#             self.threads[i].start()
#     def stop_workers(self) -> None:
#         for _ in self.threads:
#             self.queue.put(None)
#         for thread in self.threads:
#             thread.join()
#     def send_to_queue(self, funcs : List[object], params : List[tuple]) -> list:
#         results : list = [None] * len(funcs)
#         for i,func in enumerate(funcs):
#             item = (func, params[i], i, results)
#             self.queue.put(item)
#         self.queue.join()
#         return results

# def manage_workers(action : int = 0, funcs : List[object] = [], params : List[tuple] = []) -> list:
#     """Processes functions in worker processes and returns values"""
#     global workers
#     match action:
#         case 0: #start worker threads
#             workers = Workers()
#         case 1: #add items to worker threads and wait
#             return workers.send_to_queue(funcs, params)
#         case 2: #end all worker threads
#             workers.stop_workers()
#         case _:
#             pass
#     return []

