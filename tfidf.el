(defun  segment(lst)
  (if (eq lst nil)
      nil
    (if (big (car lst)
             0)
        (cons (ctoa (car lst))
              (segment (cdr lst)))
      (progn
        (cons (concat (ctoa (car lst))
                      (ctoa (car (cdr lst))))
              (segment (cdr (cdr lst))))))))

(defun  ivt(key docid)
  (add_array key docid))

(defun  handivt(lst docid)
  (if (eq lst nil)
      nil
    (progn
      (ivt (car lst)
           docid)
      (handivt (cdr lst) docid))))

(defun  wrapstdin(num line)
  (progn
    (handivt (segment (dump_bytes (car (cdr line))))
             (itoa num))
    (reactor file (add num 1) )))

(defun  reactor(file num)
  (if (feof file)
      num
    (wrapstdin
     num
     (tabsplit
      (strip (fgets file))))))

(defun balance(key)
  (if (eq (geq key) nil)
      0
    (size_array (geq key))))

(defun  tfidf_worker(docs tes termnum)
  (if (eq tes 0)
      0
    (div
     (mul (minus (log docs)
                 (log tes))
          1)
     termnum)))

(defun  estimate_worker(docs tes)
  tes)

(defun  wist(docs lst sl abc)
  (if (eq lst nil)
      nil
    (cons
     (funcall abc
              docs
              (balance (car lst))
              sl)
     (wist
      docs
      (cdr lst)
      sl))))

(defun  tfidf_accum(lst)
  lst)

(defun  estimate_accum(lst)
  (if (eq lst nil)
      (geq (quote docnum))
    (div 
     (mul (car lst)
          (estimate_accum (cdr lst)))
     (geq (quote docnum)))))

(defun  calc(docs lst)
  (print
   (estimate_accum
    (print
     (wist docs lst (size lst)
           'estimate_worker)))))

(defun  blend(query docs)
  (calc
   docs
   (segment (dump_bytes query))))

(defun  ss(docs)
  (if (eofstdin)
      nil  
    (progn
      (print (quote new_query:))
      (blend (strip (stdin)) docs) 
      (print 'LINE)
      (ss))))

(seq (quote begin) (unixtime))
(print (quote loading))
(seq (quote docnum) (reactor (fopen 'product 'r)
                             0))
(print (concat (quote page:) (geq (quote docnum))))
(print (quote loading ok))
(print (div (minus (unixtime)
                   (geq (quote begin)))
            60))

(ss (geq (quote docnum)))
