
(defun  handle(lst)
  (if (eq lst nil)
      nil
    (if (big (car lst)
             0)
        (progn
          (printchar (storage ***))        
          (printchar (car lst))
          (handle (cdr lst)))
      (progn
        (printchar (storage ***))        
        (printchar (concat (ctoa (car lst))
                           (ctoa (car (cdr lst)))))
        (handle (cdr (cdr lst)))))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (handle (dump_bytes (stdin)))
      (reactor))))

(reactor)
