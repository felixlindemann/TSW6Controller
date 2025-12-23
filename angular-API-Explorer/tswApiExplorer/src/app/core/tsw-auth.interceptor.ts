import { HttpInterceptorFn } from '@angular/common/http';

const STORAGE_KEY = 'tsw_dtg_comm_key';

export const tswAuthInterceptor: HttpInterceptorFn = (req, next) => {
  const key = localStorage.getItem(STORAGE_KEY)?.trim();
   const baseUrl = 'http://10.10.78.96:31270';

  // Only attach for our proxied API calls
  if (!req.url.startsWith(baseUrl)) {
    return next(req);
  }

  if (!key) {
    return next(req);
  }

  const cloned = req.clone({
    headers: req.headers.set('DTGCommKey', key)
  });

  return next(cloned);
};
