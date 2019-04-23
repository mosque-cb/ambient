(defun calc(avalue tmp height mid low lr c)
  (if (eq avalue tmp)
      (progn
        (add_long c tmp)
        mid)
    (if (eq low height)
        mid
      (if (big tmp avalue)
          (wraplr avalue
                  b
                  (minus mid 1)
                  low
                  lr
                  c)
        (wraplr avalue
                b
                height
                (add mid 1)
                lr
                c)))))

(defun landr(avalue b height low mid lr c)
  (calc avalue
        (idx_longs b mid)
        height
        mid
        low
        lr
        c))

(defun wraplr(avalue b height low lr c)
  (if (big low height)
      lr
    (landr avalue
           b
           height
           low
           (div (add height low)
                2)
           lr
           c)))

(defun wrapse(i a alen b br bl c)
  (if (eq alen 
          (add 1 (mul 2 i)))
      (print (quote i map))
    (se
     (add i 1)
     a
     alen
     b
     (wraplr (idx_longs a (minus alen i 1))
             b
             br
             bl
             br
             c)
     bl)))

(defun se(i a alen b br bl c)
  (if (big i (div alen 2))
      (print (quote i big))
    (if (eq alen 
            (mul 2 i))
        (print (quote i equ))
      (wrapse
       i
       a
       alen
       b
       br
       (wraplr (idx_longs a i)
               b
               br
               bl
               bl
               c)
       c))))

(defun intersection(a b c)
  (se
   0
   a
   (size_longs a)
   b
   (minus (size_longs b) 1)
   0
   c))

(defun dispatch(a b ab)
  (if (not (eq (geq ab) nil))
      ab
    (if (or (eq a nil)
            (eq  b nil))
        nil
      (progn
        (intersection
         (qsort a)
         (qsort b)
         ab)
        (print ab)))))

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
  (add_long key docid))

(defun  handivt(lst docid)
  (if (eq lst nil)
      nil
    (progn
      (ivt (car lst) docid)
      (handivt (cdr lst) docid))))

(defun  wrapstdin(line docid)
  (progn
    (if (eq (mod docid 10000)
            0)
        (print docid)
      nil)
    (add_ptr 'forward line)
    (handivt (segment (dump_bytes line)) docid)
    (reactor (add docid 1))))

(defun  reactor(docid file)
  (if (feof file)
      nil
    (wrapstdin (strip (fgets file)) docid)))

(defun  wist(a lst)
  (if (eq a nil)
      nil
    (if (eq lst nil)
        a
      (progn
        (wist (dispatch (geq a)
                        (geq (car lst))
                        (randomname))
              (cdr lst))))))

(defun  lookup(lst)
  (if (eq lst nil)
      nil
    (wist (car lst)
          (cdr lst))))

(defun  helper(col a b)
  (if (big a b)
      (progn
        (print (idx_ptrs (geq 'forward)
                         (idx_longs col b)))
        (helper col a (add b 1))
        )
    nil))

(defun  lookforward(col query)
  (progn
    (helper (geq col)  (size_longs (geq col)) 0)
    (print (tabconcat (list query
                            (size_longs (geq col)))))))

(defun  blend(query)
  (lookforward
   (lookup (segment (dump_bytes query)))
   query))

(defun  ss()
  (if (eofstdin)
      nil  
    (progn
      (print (quote new_query:))
      (blend (strip (stdin))) 
      (print 'LINE)
      (ss))))

(print (quote loading)) 
(reactor 0 (fopen 'gbk 'r))
(print (quote loading ok)) 
(ss)
