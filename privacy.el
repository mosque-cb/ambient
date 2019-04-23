(seq  redis     (dcreate))
(seq  knot     'knot)
(seq  notuniq 0)
(seq  uniq 0)

(defun mystrdup (material begin skip end)
  (if (or (eq material nil)
          (eq begin nil)
          (eq end nil))
      nil
    (strdup material 
            (add begin skip)
            end)))

(defun prepare_exact (material end)
  (if (eq material nil)
      nil
    (strdup material 0 (find material end))))

(defun process(material begin end)
  (if (eq material nil)
      nil
    (prepare_exact
     (mystrdup material
               (find material begin)
               (strlen begin)
               (strlen material))
     end)))

(defun serial(lst)
  (if (eq lst nil)
      nil
    (if (eq (car lst) nil)
        (concat 'nothing
                'TAB
                (serial (cdr lst)))
      (concat (car lst)
              'TAB
              (serial (cdr lst))))))


(defun combination(phone id userid)
  (if (eq phone nil)
      nil
    (if (eq (dget redis knot phone) 'phone)
        (progn  
          (seq notuniq (add notuniq 1))
          nil)
      (progn
        (seq uniq (add uniq 1))
        (dset  redis  
               knot 
               phone
               'phone)
        (serial (list phone id userid))))))

(defun wrapprocess(material)
  (combination
   (process material (quote ?phone_num=) (quote &))
   (process material (quote &id=) (quote &))
   (process material (quote &userid=) 'SPACE)))

(defun wrapprint(material)
  (if (eq material nil)
      nil
    (print material)))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (wrapprint (wrapprocess (strip (stdin))))
      (reactor))))

(reactor)
(printerr (concat 'uniq 'SPACE uniq))
(printerr (concat 'notuniq 'SPACE notuniq))

