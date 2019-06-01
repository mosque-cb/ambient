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

(defun  ivt(key topicid)
  (if (eq (geq key) nil)
      (add_array key topicid)
    (if (eq
         (bsearch
          (qsort (geq key))
          topicid)
         nil)
        (add_array key topicid)
      nil)))

(defun  handivt(lst topicid)
  (if (eq lst nil)
      nil
    (progn
      (ivt (car lst)
           topicid)
      (incrby (concat (quote term:)
                      (car lst))
              1)
      (incrby (concat topicid
                      (quote docterm:)
                      (car lst))
              1)
      (incrby (concat (quote doc:)
                      topicid)
              1)
      (handivt (cdr lst) topicid))))

(seq (quote topicid) 0)

(defun twist(labelkey value)
  (progn
    (seq labelkey value)
    (seq (quote topicid) value)    
    (seq (concat (quote label:)
                 (itoa value))
         labelkey)
    value))

(defun gettopicid(labelkey)
  (if (eq (geq labelkey) nil)
      (twist labelkey (add (geq (quote topicid)) 1))
    (geq labelkey)))

(defun  extractocto(three)
  (if (eq three nil)
      nil
    (gettopicid (concat (quote label:)
                        (car (octosplit three))))))

(defun  wrapstdin(num line)
  (progn
    (handivt (segment (dump_bytes (car (cdr line))))
             (extractocto
              (strip
               (car (cddddddddr line)))))
    (reactor file (add num 1) )))

(defun  reactor(file num)
  (if (feof file)
      num
    (wrapstdin
     num
     (tabsplit
      (strip (fgets file))))))


(defun  candidate(key topicid)
  (if (eq (geq key) nil)
      (add_array key topicid)
    (if (eq
         (bsearch
          (qsort (geq key))
          topicid)
         nil)
        (add_array key topicid)
      nil)))

(defun  rough(docs a b)
  (if (big b a)
      (progn
        (candidate  query
                    (idx_array docs a))
        (rough docs
               (add a 1)
               b))
    nil))

(defun  scratch(a query)
  (if (eq a nil)
      nil
    (rough a
           0
           (size_array a))))

(defun  se(lst query)
  (if (eq lst nil)
      query
    (progn
      (scratch (geq (car lst))
               query)     
      (se (cdr lst)))))

(defun balance(key)
  (if (eq (geq key) nil)
      0
    (geq key)))

(defun  myminus(a b)
  (if (big a b)
      (minus a b)
    1))

(defun  nbs(te doct doc)
  (if (eq te 0)
      0
    (if (eq doct 0)
        0
      (myminus
       (log (mul doct doc))
       (log te)))))

(comment
 (log (div (mul doct doc) te)))

(defun  relation(lst)
  (if (eq lst nil)
      1
    (mul (car lst)
         (relation (cdr lst)))))

(defun  calcivt(term topicid)
  (nbs
   (balance (concat (quote term:) term))
   (balance (concat topicid (quote docterm:) term))
   (balance (concat (quote doc:) topicid))))

(defun  wist(topicid accum lst)
  (if (eq lst nil)
      (relation (print accum))
    (wist
     topicid
     (cons
      (calcivt (car lst)
               topicid)
      accum)
     (cdr lst))))

(defun isin(a lstb)
  (if (eq lstb nil)
      nil
    (if (eq (cadr a) (cadr (car lstb)))
        1
      (isin a (cdr lstb)))))

(defun diff(lsta lstb)
  (if (eq lsta nil)
      nil
    (if (eq (isin (car lsta)
                  lstb)
            nil)
        (cons (car lsta)
              (diff (cdr lsta)
                    lstb))
      (diff (cdr lsta)
            lstb))))

(defun biggest(lsta item)
  (if (eq lsta nil)
      item
    (if (big (caar lsta)
             (car item))
        (biggest (cdr lsta)
                 (car lsta))
      (biggest (cdr lsta)
               item))))

(defun wrapabc(lsta lstb)
  (abc (diff lsta lstb)
       lstb))

(defun abc(lsta lstb)
  (if (eq lsta nil)
      (print lstb)
    (wrapabc
     lsta
     (cons  (biggest (cdr lsta)
                     (car lsta))
            lstb))))

(defun  helper(terms col a b)
  (if (big a b)
      (cons
       (list
        (wist 
         (idx_array col b)
         nil
         terms)
        (geq (concat (quote label:)
                     (itoa
                      (idx_array col b)))))
       (helper terms col a (add b 1)))
    nil))

(defun  calc(terms col)
  (abc
   (helper terms
           (geq col)
           (size_array (geq col))
           0)
   nil))

(defun  blend(query)
  (calc
   (segment (dump_bytes query))   
   (se (segment (dump_bytes query))
           query)))

(defun  ss()
  (if (eofstdin)
      nil  
    (progn
      (print (quote new_query:))
      (blend (strip (stdin))) 
      (print 'LINE)
      (ss))))

(seq (quote begin) (unixtime))
(print (quote loading))
(print (concat (quote page:)
               (reactor (fopen 'product 'r)
                        0)))
(print (quote loading ok))
(print (div (minus (unixtime)
                   (geq (quote begin)))
            60))

(ss)
