import asyncio
import concurrent.futures


async def blockingToAsync(func, *args, timeout=1000):
    result = await asyncio.wait(fs={
        asyncio.get_event_loop().
        run_in_executor(
            concurrent.futures.ThreadPoolExecutor(1),
            func, *args)}, timeout=timeout)
    funcRet = tuple(elem.result() for elem in tuple(result[0]))
    return funcRet[0] if len(funcRet) == 1 else funcRet
