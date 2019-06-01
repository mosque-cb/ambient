(defun calc(avalue tmp height mid low lr c)
  (if (eq avalue tmp)
      (progn
        (add_array c tmp)
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
        (idx_array b mid)
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
     (wraplr (idx_array a (minus alen i 1))
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
       (wraplr (idx_array a i)
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
   (size_array a)
   b
   (minus (size_array b) 1)
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

(seq (quote mode) nil)

(defun  ivt(key docid)
  (if (geq (quote mode))
      (add_array key docid)      
    (if (eq (geq key) nil)
        (add_array key docid)
      (if (eq
           (bsearch
            (qsort (geq key))
            docid)
           nil)
          (add_array key docid)
        nil))))


(defun  handivt(lst docid)
  (if (eq lst nil)
      nil
    (progn
      (ivt (car lst) docid)
      (handivt (cdr lst) docid))))

(seq (quote docid) 0)

(defun twist(labelkey value)
  (progn
    (seq labelkey value)
    (seq (quote docid) value)    
    (seq (concat (quote label:)
                 (itoa value)) labelkey)
    value))

(defun getdocid(labelkey)
  (if (eq (geq labelkey) nil)
      (twist labelkey (add (geq (quote docid)) 1))
    (geq labelkey)))

(defun  extractocto(three)
  (if (eq three nil)
      nil
    (getdocid (concat (quote label:)
                      (car (octosplit three))))))

(defun  wrapstdin(line docid)
  (progn
    (if (geq (quote mode))
        (progn
          (add_array 'forward line)
          (handivt (segment (dump_bytes (car (cdr line)))) docid))
      (progn
        (handivt (segment (dump_bytes (car (cdr line))))
                 (extractocto
                  (car (cdr 
                        (cdr (cdr
                              (cdr (cdr (cdr (cdr (cdr line)))))))))))))
    (reactor (add docid 1))))

(defun  reactor(docid file)
  (if (feof file)
      nil
    (wrapstdin (tabsplit (strip (fgets file)))
               docid)))

(defun  wist(a lst)
  (if (eq a nil)
      nil
    (if (eq lst nil)
        a
      (progn
        (wist (dispatch (geq a)
                        (geq (car lst))
                        (concat a (car lst)))                        
              (cdr lst))))))

(defun  lookup(lst)
  (if (eq lst nil)
      nil
    (wist (car lst)
          (cdr lst))))

(defun  helper(col a b)
  (if (big a b)
      (progn
        (if (geq (quote mode))
            (print (idx_array (geq 'forward)
                              (idx_array col b)))
          (print (geq (concat (quote label:)
                              (itoa
                               (idx_array col b))))))
        (helper col a (add b 1))
        )
    nil))

(defun  lookforward(col query)
  (progn
    (helper (geq col)  (size_array (geq col)) 0)
    (print (tabconcat (list query
                            (size_array (geq col)))))))

(defun  blend(query)
  (lookforward
   (lookup (segment (dump_bytes (car (cdr query)))))
   query))

(defun  ss()
  (if (eofstdin)
      nil  
    (progn
      (print (quote new_query:))
      (blend (tabsplit (strip (stdin))))
      (print 'LINE)
      (ss))))

(seq (quote begin) (unixtime))
(print (quote loading)) 
(reactor 0 (fopen 'product 'r))
(print (quote loading ok))
(print (div (minus (unixtime)
                   (geq (quote begin)))
            60))

(ss)
