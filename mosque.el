(defun  mycons ( x  y)
  (lambda (p ) (funcall p  
                        x  
                        y)))

(defun  mycar  (n)
  (funcall  n   
            (lambda ( x y)x)))

(defun  mycdr  (n)
  (funcall  n   
            (lambda( x y)y)))

(defun  mycadr  (n)
  (mycar  (mycdr  n)))

(defun  mycdar  (n)
  (mycdr  (mycar  n)))

(defun printlst (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print  (mycar  n) )
      (printlst (mycdr n) ))))

(defun  form_struct (lst)
  (if  (eq  lst  nil)
      nil
    (if  (atom lst)
        lst
      (mycons  (form_struct (car lst))
               (form_struct (cdr lst))))))

(defun  wrap_defun (lst )
  (progn
    (seq  global_defun  (mycons  lst global_defun))
    (print  'wrap_defun)))

(defun  funp(name global)
  (if  (eq  global  nil)
      0
    (if  (eq  name  (mycar (mycar global)))
        1
      (funp  name  (mycdr  global)))))

(defun  find_expr(name global)
  (if  (eq  global  nil)
      nil
    (if  (eq  name  (mycar (mycar global)))
        (mycadr (mycdar global))
      (find_expr  name  (mycdr  global)))))

(defun  find_arg(name global)
  (if  (eq  global  nil)
      nil
    (if  (eq  name  (mycar (mycar global)))
        (mycadr (mycar global))
      (find_arg  name  (mycdr  global)))))

(defun  bindvars  (arg value env)
  (mycons
   (bindvars_help arg value)
   env))

(defun  bindvars_help  (arg value )
  (if  (eq  arg  nil)
      nil
    (mycons  ( mycons  (mycar arg)  (mycons (mycar value) nil))
             (bindvars_help  (mycdr  arg) (mycdr value)))))

(defun  var_find (arg env)
  (if  (eq  env  nil)
      0
    (if  (eq  arg  (mycar  (mycar env)))
        1
      (var_find arg  (mycdr env)))))

(defun  var_fetch (arg env)
  (if  (eq  env  nil)
      nil
    (if  (eq  arg  (mycar  (mycar env)))
        (mycadr  (mycar env))
      (var_fetch arg  (mycdr env)))))

(defun  findvar  (arg  env)
  (if  (eq  env  nil)
      nil
    (if  (var_find   arg    (mycar env))
        (var_fetch   arg    (mycar env))
      (findvar  arg  (mycdr env)))))

(defun   eval_para (lst env cont)
  (if  (eq lst nil)
      (funcall  cont nil)
    (eval_para  (mycdr lst)
                env  
                (lambda (y)
                  (wrap_eval  (mycar lst)  
                              env   
                              (lambda  (x) 
                                (progn
                                  (funcall  cont 
                                            (mycons  x y)))))))))

(defun   wrap_eval (lst env cont)
  (if  (eq  lst nil)
      (funcall  cont lst)
    (if  (digitp  lst )
        (funcall cont lst)
      (if  (charp  lst)
          (progn
            (funcall  cont   (findvar lst 
                                      env)))
        (if  (eq (mycar lst)  'mod)
            (eval_para  (mycdr lst) 
                        env  
                        (lambda  (lst ) 
                          (funcall cont  
                                   (mod  (mycar lst) 
                                         (mycadr lst)))))
          (if  (eq (mycar lst)  'add)
              (progn
                (eval_para  (mycdr lst) 
                            env  
                            (lambda  (lst )
                              (funcall  cont  
                                        (progn 
                                          (add  (mycar lst) 
                                                (mycadr lst)))))))
            (if  (eq (mycar lst)  'minus)
                (eval_para  (mycdr lst) 
                            env  
                            (lambda  (lst )
                              (funcall  cont 
                                        (minus  (mycar lst) 
                                                (mycadr lst)))))
              (if  (eq (mycar lst)  'random)
                  (funcall cont		  
                           (random  (mycadr lst ) ))
                (if  (eq (mycar lst)  'mycons)
                    (eval_para  (mycdr lst) 
                                env 
                                (lambda  (lst ) 
                                  (progn
                                    (funcall  cont  
                                              (mycons  (mycar lst) 
                                                       (mycadr lst))))))
                  (if  (eq (mycar lst)  'mycdr)
                      (eval_para  (mycdr lst) 
                                  env 
                                  (lambda  (lst ) 
                                    (funcall  cont  
                                              (mycdr  (mycar lst)))))
                    (if  (eq (mycar lst)  'mycar)
                        (eval_para  (mycdr lst) 
                                    env 
                                    (lambda  (lst ) 
                                      (funcall  cont  
                                                (mycar  (mycar lst)))))
                      (if  (eq (mycar lst)  'printlst)
                          (eval_para  (mycdr lst) 
                                      env 
                                      (lambda  (lst ) 
                                        (printlst  (mycar lst ))
                                        (funcall  cont (mycar lst))))
                        (if  (eq (mycar lst)  'if)
                            (wrap_eval  (mycar (mycdr lst))  
                                        env 
                                        (lambda  ( x)  
                                          (if   x
                                              (wrap_eval  (mycadr (mycdr lst)) 
                                                          env 
                                                          cont)
                                            (wrap_eval  
                                             (mycadr 
                                              (mycdr (mycdr lst))) 
                                             env 
                                             cont))))
                          (if  (eq (mycar lst)  'eq)
                              (eval_para  (mycdr lst) 
                                          env  
                                          (lambda  (lst )
                                            (if (eq  (mycar lst) 
                                                     (mycadr lst))
                                                (funcall  cont  1)
                                              (funcall  cont  0))))
                            (if  (eq (mycar lst)  'defun)
                                (funcall  cont		
                                          (wrap_defun  (mycdr lst ) ))
                              (if  (funp  (mycar lst) global_defun)
                                  (eval_para 
                                   (mycdr lst) 
                                   env
                                   (lambda ( parame )
                                     (wrap_eval  (find_expr  (mycar lst) 
                                                             global_defun)
                                                 (bindvars   (find_arg (mycar lst) 
                                                                       global_defun) 
                                                             parame
                                                             env  )
                                                 (lambda (result ) 
                                                   (progn
                                                     (funcall cont 
                                                              result))))))
                                nil))))))))))))))))

(defun  generand  (count range)
  (if  (eq  count  0)
      nil
    (mycons    (random  range)
               (generand  (minus  count  1)
                          range))))

(defun once  ( )
  (progn 
    (display )
    (seq  global_defun  nil)
    (main basic_defun )
    (main basic_apply )
    (seq  global_defun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (quote times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (wrap_eval  (mycar lst) nil (lambda (x)  (print x))  )
      (main (mycdr lst)  ))))

(seq  global_defun  nil)
(define  basic_defun   '(  (defun  sum (a b)
                             (add  a  b))

                           (defun  fibs  (n )
                             (if  (eq  n  1)
                                 1
                               (if  (eq  n  2)
                                   2
                                 (add (fibs  (minus n  1))
                                      (fibs (minus n 2))))))

                           (defun  reactor (from  to )
                             (if  (eq  from  to)
                                 nil
                               (mycons  from  
                                        (reactor  (add  from  1)
                                                  to))))

                           (defun  mymod  (m n)
                             (if  (mod  m n)
                                 1
                               0))

                           (defun  filter (  a  s)
                             (if  (eq  s  nil)
                                 nil
                               (if  (mymod  (mycar s) a)
                                   (mycons  (mycar s)
                                            (filter   a  (mycdr s)))
                                 (filter   a  (mycdr s)))))

                           (defun  prims (s)
                             (if (eq s  nil)
                                 nil
                               (mycons (mycar s)
                                       (prims  (filter  (mycar s)
                                                        (mycdr s))))))
                           ))


(define  basic_apply    '( 
                          (printlst  (prims (reactor  1  
                                                      (sum 1 
                                                           (fibs (random 3))))))
                          ))

(comment
(define  basic_apply    '( 
                          (printlst  (reactor  1  (random 8)))
                          ))
)

(define  basic_show    'hello)
(print   'init_object)
(once)
(comment (autotest  0))
