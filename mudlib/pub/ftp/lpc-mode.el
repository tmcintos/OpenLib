;;;;;;;;;;;;;;;;;;;;;;;;;;; -*- Mode: Emacs-Lisp -*- ;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 
;; lpc-mode.el - Major mode for editing LPC code
;; 
;; Copyright (C) 1991 Kresten Krab Thorup (krab@iesd.auc.dk).
;; 
;; Heavily based on C++ mode by Dave Detlefs (dld@cs.cmu.edu) 
;; and  Stewart Clamen (clamen@cs.cmu.edu).
;; 
;; $Id: $
;; 
;; LCD Archive Entry:
;; lpc-mode.el|Kresten Krab Thorup|krab@iesd.auc.dk
;; | ...description...
;; |$Date$|$Revision$|iesd.auc.dk:/pub/emacs-lisp/lpc-mode.el
;; 
;; Author          : Kresten Krab Thorup
;; Created On      : Mon Feb 24 10:08:15 1992
;; Last Modified By: Stig Sæther Bakken
;; Last Modified On: Tue May 31 01:08:22 1994
;; Update Count    : 14
;; 
;; HISTORY
;;
;; 940531 - Added hilit19 support, menubar support and (provide).
;;          (Stig.Bakken@pvv.unit.no)
;; 940709 - Added option for turning off insert-mode-line.
;;          Updated the hilit regexp for LPC4's types and keywords.
;;          (Stig.Bakken@pvv.unit.no)
;; 940719 - Added some support for LPMud 3.2's closure syntax.
;;          (Stig.Bakken@pvv.unit.no)
;; 941225 - Added support for MudOS's efuns.
;;          (Stig.Bakken@pvv.unit.no)
;; 

(defvar lpc-mode-syntax-table nil
  "Syntax table in use in LPC-mode buffers.")
(defvar lpc-mode-abbrev-table nil
  "Abbrev table in use in LPC-mode buffers.")

(defvar lpc-mode-map (make-sparse-keymap)
  "Keymap used in LPC mode.")

(defvar dont-insert-mode-line nil
  "Prevents the mode line from being inserted at the top of files.")

(define-key lpc-mode-map "\C-j" 'reindent-then-newline-and-indent)
(define-key lpc-mode-map "{" 'electric-lpc-brace)
(define-key lpc-mode-map "}" 'electric-lpc-brace)
(define-key lpc-mode-map ";" 'electric-lpc-semi)
(define-key lpc-mode-map "\e\C-h" 'mark-c-function)
(define-key lpc-mode-map "\e\C-q" 'indent-lpc-exp)
(define-key lpc-mode-map "\177" 'backward-delete-char-untabify)
(define-key lpc-mode-map "\t" 'lpc-indent-command)
(define-key lpc-mode-map "\C-c\C-i" 'lpc-insert-header)
(define-key lpc-mode-map "\C-c\C-\\" 'lpc-macroize-region)
(define-key lpc-mode-map "\e\C-a" 'lpc-beginning-of-defun)
(define-key lpc-mode-map "\e\C-e" 'lpc-end-of-defun)
(define-key lpc-mode-map "\e\C-q" 'lpc-indent-defun)

(define-key lpc-mode-map [menu-bar] (make-sparse-keymap))

(define-key lpc-mode-map [menu-bar lpc]
  (cons "LPC" (make-sparse-keymap "LPC")))

(define-key lpc-mode-map [menu-bar lpc comment-region]
  '("Comment Out Region"     . comment-region))
(define-key lpc-mode-map [menu-bar lpc c-macro-expand]
  '("Macro Expand Region"    . c-macro-expand))
(define-key lpc-mode-map [menu-bar lpc c-backslash-region]
  '("Backslashify"           . c-backslash-region))
(define-key lpc-mode-map [menu-bar lpc indent-exp]
  '("Indent Expression"      . indent-lpc-exp))
(define-key lpc-mode-map [menu-bar lpc indent-line]
  '("Indent Line"            . lpc-indent-command))
(define-key lpc-mode-map [menu-bar lpc fill]
  '("Fill Comment Paragraph" . c-fill-paragraph))
(define-key lpc-mode-map [menu-bar lpc up]
  '("Up Conditional"         . c-up-conditional))
(define-key lpc-mode-map [menu-bar lpc backward]
  '("Backward Conditional"   . c-backward-conditional))
(define-key lpc-mode-map [menu-bar lpc forward]
  '("Forward Conditional"    . c-forward-conditional))
(define-key lpc-mode-map [menu-bar lpc backward-stmt]
  '("Backward Statement"     . c-beginning-of-statement))
(define-key lpc-mode-map [menu-bar lpc forward-stmt]
  '("Forward Statement"      . c-end-of-statement))


(defvar lpc-friend-offset -4
  "*Offset of LPC friend class declarations relative to member declarations.")
(defvar lpc-electric-colon t
  "*If t, colon is an electric terminator.")
(defvar lpc-empty-arglist-indent nil
  "*Indicates how far to indent an line following an empty argument
list.  Nil indicates to just after the paren.")



(defun insert-mode-line ()
    "This little macro inserts `-*- mode-name -*-' if not present.
You should insert this in your whatever-mode-hook!"
    (interactive)
    (save-excursion
      (goto-char (point-min))
      (let ((end (progn (end-of-line)
		       (point))))
	(goto-char (point-min))
	(if (not (re-search-forward "-\\*-.*-\\*-" end t))
	    (insert-string (concat comment-start
				   " -*- " mode-name " -*- "
				   comment-end "\n"))))))
 
(defun lpc-mode ()
  "Major mode for editing LPC code.  Very much like editing C code.
Expression and list commands understand all LPC brackets.
Tab at left margin indents for LPC code
Comments are delimited with /* ... */ 
Paragraphs are separated by blank lines only.
Delete converts tabs to spaces as it moves back.
\\{lpc-mode-map}
Variables controlling indentation style:
 c-tab-always-indent
    Non-nil means TAB in C mode should always reindent the current line,
    regardless of where in the line point is when the TAB command is used.
    Default is t.
 c-auto-newline
    Non-nil means automatically newline before and after braces,
    and after colons and semicolons, inserted in C code.
 c-indent-level
    Indentation of C statements within surrounding block.
    The surrounding block's indentation is the indentation
    of the line on which the open-brace appears.
 c-continued-statement-offset
    Extra indentation given to a substatement, such as the
    then-clause of an if or body of a while.
 c-brace-offset
    Extra indentation for line if it starts with an open brace.
 c-brace-imaginary-offset
    An open brace following other text is treated as if it were
    this far to the right of the start of its line.
 c-argdecl-indent
    Indentation level of declarations of C function arguments.
 c-label-offset
    Extra indentation for line that is a label, or case or default.
 lpc-electric-colon
    If non-nil at invocation of lpc-mode (t is the default) colon electricly
    indents.
 lpc-empty-arglist-indent
    If non-nil, a function declaration or invocation which ends a line with a
    left paren is indented this many extra spaces, instead of flush with the
    left paren.
 lpc-friend-offset
    Offset of LPC friend class declarations relative to member declarations.

Turning on LPC mode calls the value of the variable lpc-mode-hook with
no args,if that value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map lpc-mode-map)
  (setq major-mode 'lpc-mode)
  (setq mode-name "LPC")
  (define-abbrev-table 'lpc-mode-abbrev-table ())
  (setq local-abbrev-table lpc-mode-abbrev-table)
  (if (not lpc-mode-syntax-table)
      (let ((i 0))
	(setq lpc-mode-syntax-table (make-syntax-table))
	(set-syntax-table lpc-mode-syntax-table)
	(modify-syntax-entry ?\\ "\\")
	(modify-syntax-entry ?/ ". 12")
	(modify-syntax-entry ?\n ">")
	(modify-syntax-entry ?+ ".")
	(modify-syntax-entry ?- ".")
	(modify-syntax-entry ?= ".")
	(modify-syntax-entry ?% ".")
	(modify-syntax-entry ?< ".")
	(modify-syntax-entry ?> "."))
      (set-syntax-table lpc-mode-syntax-table))
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "^$\\|" page-delimiter))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'lpc-indent-line)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'comment-start)
  (setq comment-start "/* ")
  (make-local-variable 'comment-end)
  (setq comment-end " */")
  (make-local-variable 'comment-column)
  (setq comment-column 32)
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "/\\*+ *")
  (make-local-variable 'comment-indent-function)
  (setq comment-indent-function 'lpc-comment-indent)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments nil)
  (run-hooks 'lpc-mode-hook)
  (if (not dont-insert-mode-line)
      (insert-mode-line))
  (if lpc-electric-colon
      (define-key lpc-mode-map ":" 'electric-lpc-terminator)))


;; This is used by indent-for-comment
;; to decide how much to indent a comment in LPC code
;; based on its context.

(defun lpc-comment-indent ()
  (let (cur-col)
    (save-excursion
      (skip-chars-backward " \t")
      (setq cur-col (current-column))
      (end-of-line 0)
      (if (re-search-backward comment-start-skip
			      (save-excursion (beginning-of-line 1) (point))
			      t)
	  (if (= (current-column) 0) 0
	    (max (1+ cur-col) (current-column)))
	; Else indent at comment column, except leave at least one space.
	(max (1+ cur-col) comment-column)))))

(defun electric-lpc-brace (arg)
  "Insert character and correct line's indentation."
  (interactive "P")
  (let (insertpos)
    (if (and (not arg)
	     (eolp)
	     (or (save-excursion
		   (skip-chars-backward " \t")
		   (bolp))
		 (if c-auto-newline
		     (progn (lpc-indent-line) (newline) t)
		   nil)))
	(progn
	  (insert last-command-char)
	  (lpc-indent-line)
	  (if c-auto-newline
	      (progn
		(setq insertpos (1- (point)))
		(newline)
		(lpc-indent-line)))
	  (save-excursion
	    (if insertpos (goto-char (1+ insertpos)))
	    (delete-char -1))))
    (if insertpos
	(save-excursion
	  (goto-char insertpos)
	  (self-insert-command (prefix-numeric-value arg)))
      (self-insert-command (prefix-numeric-value arg)))))

(defun electric-lpc-semi (arg)
  "Insert character and correct line's indentation."
  (interactive "P")
  (if c-auto-newline
      (electric-lpc-terminator arg)
    (self-insert-command (prefix-numeric-value arg))))

(defun electric-lpc-terminator (arg)
  "Insert character and correct line's indentation."
  (interactive "P")
  (let (insertpos (end (point)))
    (if (and (not arg) (eolp)
	     (not (save-excursion
		    (beginning-of-line)
		    (skip-chars-forward " \t")
		    (or (= (following-char) ?#)
			(let ((pps (parse-partial-sexp (point) end)))
			  (or (nth 3 pps) (nth 4 pps) (nth 5 pps)))))))
	(progn
	  (insert last-command-char)
	  (lpc-indent-line)
	  (and c-auto-newline
	       (not (c-inside-parens-p))
	       (progn
		 (setq insertpos (1- (point)))
		 (newline)
		 (lpc-indent-line)))
	  (save-excursion
	    (if insertpos (goto-char (1+ insertpos)))
	    (delete-char -1))))
    (if insertpos
	(save-excursion
	  (goto-char insertpos)
	  (self-insert-command (prefix-numeric-value arg)))
      (self-insert-command (prefix-numeric-value arg)))))

(defun c-inside-parens-p ()
  (condition-case ()
      (save-excursion
	(save-restriction
	  (narrow-to-region (point)
			    (progn (beginning-of-defun) (point)))
	  (goto-char (point-max))
	  (= (char-after (or (scan-lists (point) -1 1) (point-min))) ?\()))
    (error nil)))

(defun lpc-indent-command (&optional whole-exp)
  (interactive "P")
  "Indent current line as LPC code, or in some cases insert a tab character.
If c-tab-always-indent is non-nil (the default), always indent current line.
Otherwise, indent the current line only if point is at the left margin
or in the line's indentation; otherwise insert a tab.

A numeric argument, regardless of its value,
means indent rigidly all the lines of the expression starting after point
so that this line becomes properly indented.
The relative indentation among the lines of the expression are preserved."
  (if whole-exp
      ;; If arg, always indent this line as C
      ;; and shift remaining lines of expression the same amount.
      (let ((shift-amt (lpc-indent-line))
	    beg end)
	(save-excursion
	  (if c-tab-always-indent
	      (beginning-of-line))
	  (setq beg (point))
	  (forward-sexp 1)
	  (setq end (point))
	  (goto-char beg)
	  (forward-line 1)
	  (setq beg (point)))
	(if (> end beg)
	    (indent-code-rigidly beg end shift-amt "#")))
    (if (and (not c-tab-always-indent)
	     (save-excursion
	       (skip-chars-backward " \t")
	       (not (bolp))))
	(insert-tab)
      (lpc-indent-line))))

(defun lpc-indent-line ()
  "Indent current line as LPC code.
Return the amount the indentation changed by."
  (let ((indent (calculate-lpc-indent nil))
	beg shift-amt
	(case-fold-search nil)
	(pos (- (point-max) (point))))
    (beginning-of-line)
    (setq beg (point))
    (cond ((eq indent nil)
	   (setq indent (current-indentation)))
	  ((eq indent t)
	   (setq indent (calculate-c-indent-within-comment)))
	  ; There is a difference between # and #' (LPMud 3.2 closure):
	  ((looking-at "[ \t]*#[^']")
	   (setq indent 0))
	  (t
	   (skip-chars-forward " \t")
	   (if (listp indent) (setq indent (car indent)))
	   (cond ((looking-at "\\(public\\|private\\|protected\\):")
		  (setq indent (+ indent c-label-offset)))
		 ((or (looking-at "case\\b")
		      (and (looking-at "[A-Za-z]")
			   (save-excursion
			     (forward-sexp 1)
			     (looking-at ":[^:]"))))
		  (setq indent (max 1 (+ indent c-label-offset))))
		 ((looking-at "else\\b")
		  (setq indent (save-excursion
				 (c-backward-to-start-of-if)
				 (current-indentation))))
		 ((looking-at "friend\[ \t]class[ \t]")
		  (setq indent (+ indent lpc-friend-offset)))
		 ((= (following-char) ?})
		  (setq indent (- indent c-indent-level)))
		 ((= (following-char) ?{)
		  (setq indent (+ indent c-brace-offset))))))
    (skip-chars-forward " \t")
    (setq shift-amt (- indent (current-column)))
    (if (zerop shift-amt)
	(if (> (- (point-max) pos) (point))
	    (goto-char (- (point-max) pos)))
      (delete-region beg (point))
      (indent-to indent)
      ;; If initial point was within line's indentation,
      ;; position after the indentation.  Else stay at same point in text.
      (if (> (- (point-max) pos) (point))
	  (goto-char (- (point-max) pos))))
    shift-amt))

(defun calculate-lpc-indent (&optional parse-start)
  "Return appropriate indentation for current line as LPC code.
In usual case returns an integer: the column to indent to.
Returns nil if line starts inside a string, t if in a comment."
  (save-excursion
    (beginning-of-line)
    (let ((indent-point (point))
	  (case-fold-search nil)
	  state
	  containing-sexp)
      (if parse-start
	  (goto-char parse-start)
	(beginning-of-defun))
      (while (< (point) indent-point)
	(setq parse-start (point))
	(setq state (parse-partial-sexp (point) indent-point 0))
	(setq containing-sexp (car (cdr state))))
      (cond ((or (nth 3 state) (nth 4 state))
	     ;; return nil or t if should not change this line
	     (nth 4 state))
	    ((null containing-sexp)
	     ;; Line is at top level.  May be data or function definition,
	     ;; or may be function argument declaration.
	     ;; Indent like the previous top level line
	     ;; unless that ends in a closeparen without semicolon,
	     ;; in which case this line is the first argument decl.
	     (goto-char indent-point)
	     (skip-chars-forward " \t")
	     (if (= (following-char) ?{)
		 0   ; Unless it starts a function body
	       (lpc-backward-to-noncomment (or parse-start (point-min)))
	       (if (= (preceding-char) ?\))
		   c-argdecl-indent
		 (if (= (preceding-char) ?\;)
		     (backward-char 1))
		 (if (= (preceding-char) ?})
		     0
		   (current-indentation))
		 )))
	    ((/= (char-after containing-sexp) ?{)
	     ;; line is expression, not statement:
	     ;; indent to just after the surrounding open -- unless
	     ;; empty arg list, in which case we do what
	     ;; lpc-empty-arglist-indent says to do.
	     (if (and (null (nth 2 state))	;; indicates empty arg list.
		      lpc-empty-arglist-indent)
		 (progn
		   (goto-char containing-sexp)
		   (beginning-of-line)
		   (skip-chars-forward " \t")
		   (goto-char (min (+ (point) lpc-empty-arglist-indent)
				   (1+ containing-sexp)))
		   (current-column))
	     (goto-char (1+ containing-sexp))
	     (current-column)))
	    (t
	     ;; Statement.  Find previous non-comment character.
	     (goto-char indent-point)
	     (lpc-backward-to-noncomment containing-sexp)
	     (if (not (memq (preceding-char) '(nil ?\, ?\; ?} ?: ?\{)))
		 ;; This line is continuation of preceding line's statement;
		 ;; indent  c-continued-statement-offset  more than the
		 ;; previous line of the statement.
		 (progn
		   (c-backward-to-start-of-continued-exp containing-sexp)
		   (+ c-continued-statement-offset (current-column)))
	       ;; This line starts a new statement.
	       ;; Position following last unclosed open.
	       (goto-char containing-sexp)
	       ;; Is line first statement after an open-brace?
	       (or
		 ;; If no, find that first statement and indent like it.
		 (save-excursion
		   (forward-char 1)
		   (while (progn (skip-chars-forward " \t\n")
				 (looking-at
				  (concat
				   "#\\|/\\*\\|//"
				   "\\|case[ \t]"
				   "\\|[a-zA-Z0-9_$]*:[^:]")))
		     ;; Skip over comments and labels following openbrace.
		     (cond ((= (following-char) ?\#)
			    (forward-line 1))
			   ((looking-at "/\\*")
			    (search-forward "*/" nil 'move))
			   (t
			    (re-search-forward ":[^:]" nil 'move))))
		   ;; The first following code counts
		   ;; if it is before the line we want to indent.
		   (and (< (point) indent-point)
			(current-column)))
		 ;; If no previous statement,
		 ;; indent it relative to line brace is on.
		 ;; For open brace in column zero, don't let statement
		 ;; start there too.  If c-indent-offset is zero,
		 ;; use c-brace-offset + c-continued-statement-offset instead.
		 ;; For open-braces not the first thing in a line,
		 ;; add in c-brace-imaginary-offset.
		 (+ (if (and (bolp) (zerop c-indent-level))
			(+ c-brace-offset c-continued-statement-offset)
		      c-indent-level)
		    (if (save-excursion (skip-chars-backward " \t")
					(bolp))
			0 c-brace-imaginary-offset)
		    (current-indentation)))))))))

(defun calculate-c-indent-within-comment ()
  "Return the indentation amount for line, assuming that
the current line is to be regarded as part of a block comment."
  (let (end star-start)
    (save-excursion
      (beginning-of-line)
      (skip-chars-forward " \t")
      (setq star-start (= (following-char) ?\*))
      (skip-chars-backward " \t\n")
      (setq end (point))
      (beginning-of-line)
      (skip-chars-forward " \t")
      (and (re-search-forward "/\\*[ \t]*" end t)
	   star-start
	   (goto-char (1+ (match-beginning 0))))
      (current-column))))


(defun lpc-backward-to-noncomment (lim)
  (let (opoint stop)
    (while (not stop)
      (skip-chars-backward " \t\n\r\f" lim)
      (setq opoint (point))
      (cond ((and (>= (point) (+ 2 lim))
		  (save-excursion
		    (forward-char -2)
		    (looking-at "\\*/")))
	     (search-backward "/*" lim 'move))
	    ((and
	      (search-backward "//" (max (point-bol) lim) 'move)
	      (not (within-string-p (point) opoint))))
	  (t (beginning-of-line)
	     (skip-chars-forward " \t")
	     (if (looking-at "#")
		 (setq stop (<= (point) lim))
	       (setq stop t)
	       (goto-char opoint)))))))

(defun c-backward-to-start-of-continued-exp (lim)
  (if (= (preceding-char) ?\))
      (forward-sexp -1))
  (beginning-of-line)
  (if (<= (point) lim)
      (goto-char (1+ lim)))
  (skip-chars-forward " \t"))

(defun c-backward-to-start-of-if (&optional limit)
  "Move to the start of the last ``unbalanced'' if."
  (or limit (setq limit (save-excursion (beginning-of-defun) (point))))
  (let ((if-level 1)
	(case-fold-search nil))
    (while (not (zerop if-level))
      (backward-sexp 1)
      (cond ((looking-at "else\\b")
	     (setq if-level (1+ if-level)))
	    ((looking-at "if\\b")
	     (setq if-level (1- if-level)))
	    ((< (point) limit)
	     (setq if-level 0)
	     (goto-char limit))))))


(defun indent-lpc-exp ()
  "Indent each line of the LPC grouping following point."
  (interactive)
  (let ((indent-stack (list nil))
	(contain-stack (list (point)))
	(case-fold-search nil)
	restart outer-loop-done inner-loop-done state ostate
	this-indent last-sexp
	at-else
	(opoint (point))
	(next-depth 0))
    (save-excursion
      (forward-sexp 1))
    (save-excursion
      (setq outer-loop-done nil)
      (while (and (not (eobp)) (not outer-loop-done))
	(setq last-depth next-depth)
	;; Compute how depth changes over this line
	;; plus enough other lines to get to one that
	;; does not end inside a comment or string.
	;; Meanwhile, do appropriate indentation on comment lines.
	(setq innerloop-done nil)
	(while (and (not innerloop-done)
		    (not (and (eobp) (setq outer-loop-done t))))
	  (setq ostate state)
	  (setq state (parse-partial-sexp (point) (progn (end-of-line) (point))
					  nil nil state))
	  (setq next-depth (car state))
	  (if (and (car (cdr (cdr state)))
		   (>= (car (cdr (cdr state))) 0))
	      (setq last-sexp (car (cdr (cdr state)))))
	  (if (or (nth 4 ostate))
	      (lpc-indent-line))
	  (if (or (nth 3 state))
	      (forward-line 1)
	    (setq innerloop-done t)))
	(if (<= next-depth 0)
	    (setq outer-loop-done t))
	(if outer-loop-done
	    nil
	  (if (/= last-depth next-depth)
	      (setq last-sexp nil))
	  (while (> last-depth next-depth)
	    (setq indent-stack (cdr indent-stack)
		  contain-stack (cdr contain-stack)
		  last-depth (1- last-depth)))
	  (while (< last-depth next-depth)
	    (setq indent-stack (cons nil indent-stack)
		  contain-stack (cons nil contain-stack)
		  last-depth (1+ last-depth)))
	  (if (null (car contain-stack))
	      (setcar contain-stack (or (car (cdr state))
					(save-excursion (forward-sexp -1)
							(point)))))
	  (forward-line 1)
	  (skip-chars-forward " \t")
	  (if (eolp)
	      nil
	    (if (and (car indent-stack)
		     (>= (car indent-stack) 0))
		;; Line is on an existing nesting level.
		;; Lines inside parens are handled specially.
		(if (/= (char-after (car contain-stack)) ?{)
		    (setq this-indent (car indent-stack))
		  ;; Line is at statement level.
		  ;; Is it a new statement?  Is it an else?
		  ;; Find last non-comment character before this line
		  (save-excursion
		    (setq at-else (looking-at "else\\W"))
		    (lpc-backward-to-noncomment opoint)
		    (if (not (memq (preceding-char) '(nil ?\, ?\; ?} ?: ?{)))
			;; Preceding line did not end in comma or semi;
			;; indent this line  c-continued-statement-offset
			;; more than previous.
			(progn
			  (c-backward-to-start-of-continued-exp
			   (car contain-stack))
			  (setq this-indent
				(+ c-continued-statement-offset
				   (current-column))))
		      ;; Preceding line ended in comma or semi;
		      ;; use the standard indent for this level.
		      (if at-else
			  (progn (c-backward-to-start-of-if opoint)
				 (setq this-indent (current-indentation)))
			(setq this-indent (car indent-stack))))))
	      ;; Just started a new nesting level.
	      ;; Compute the standard indent for this level.
	      (let ((val (calculate-lpc-indent
			  (if (car indent-stack)
			      (- (car indent-stack))))))
		(setcar indent-stack
			(setq this-indent val))))
	    ;; Adjust line indentation according to its contents
	    (if (looking-at "\\(public\\|private\\|protected\\):")
		(setq this-indent (- this-indent c-indent-level)))
	    (if (and (looking-at "[A-Za-z]")
		     (save-excursion
		       (forward-sexp 1)
		       (looking-at ":[^:]")))
		(setq this-indent (max 1 (+ this-indent c-label-offset))))
	    (if (looking-at "case[ \t]")
		(setq this-indent (- this-indent c-indent-level)))
	    (if (looking-at "friend[ \t]class[ \t]")
		(setq this-indent (- this-indent c-indent-level)))
	    (if (= (following-char) ?})
		(setq this-indent (- this-indent c-indent-level)))
	    (if (= (following-char) ?{)
		(setq this-indent (+ this-indent c-brace-offset)))
	    ;; Put chosen indentation into effect.
	    (or (= (current-column) this-indent)
		(= (following-char) ?\#)
		(progn
		  (delete-region (point) (progn (beginning-of-line) (point)))
		  (indent-to this-indent)))
	    ;; Indent any comment following the text.
	    (or (looking-at comment-start-skip)
		(if (re-search-forward
		     comment-start-skip
		     (save-excursion (end-of-line) (point))
		     t)
		    (progn (indent-for-comment) (beginning-of-line))))))))))

(defun fill-C-comment ()
  (interactive)
  (save-excursion
    (let ((save fill-prefix))
      (beginning-of-line 1)
      (save-excursion
	(re-search-forward comment-start-skip
			   (save-excursion (end-of-line) (point))
			   t)
	(goto-char (match-end 0))
	(set-fill-prefix))
      (while (looking-at fill-prefix)
	(previous-line 1))
      (next-line 1)
      (insert-string "\n")
      (fill-paragraph nil)
      (delete-char -1)
      (setq fill-prefix save))))

(defun point-bol ()
  "Returns the value of the point at the beginning of the current
line."
  (save-excursion
    (beginning-of-line)
    (point)))

(defun previous-line-starts-with (string)
  "Returns t if previous nonblank line begins with STRING"
  (save-excursion
    (beginning-of-line 0)
    (skip-chars-forward " \t")
    (if (eolp)
	(previous-line-starts-with string)
      (looking-at string))))

(defun lpc-insert-header ()
  "Insert header denoting LPC code at top of buffer."
  (interactive)
  (save-excursion
    (goto-char (point-min))
    (insert "// "
	    "This may look like C code, but it is really "
	    "-*- LPC -*-"
	    "\n\n")))


(defun within-string-p (point1 point2)
  "Returns true if number of double quotes between two points is odd."
  (let ((s (buffer-substring point1 point2)))
    (not (zerop (mod (count-char-in-string ?\" s) 2)))))

(defun count-char-in-string (c s)
  (let ((count 0)
	(pos 0))
    (while (< pos (length s))
      (setq count (+ count (if (\= (aref s pos) c) 1 0)))
      (setq pos (1+ pos)))
    count))


;;; This page covers "macroization;" making LPC parameterized types
;;; via macros.

(defvar lpc-default-macroize-column 78
  "Place to insert backslashes.")

(defun lpc-macroize-region (from to arg)
  "Insert backslashes at end of every line in region.  Useful for defining cpp
macros.  If called with negative argument, will remove trailing backslashes,
so that indentation will work right."
  (interactive "r\np")
  (save-excursion
    (goto-char from)
    (beginning-of-line 1)
    (let ((line (count-lines (point-min) (point)))
	  (to-line (save-excursion (goto-char to)
				   (count-lines (point-min) (point)))))
      (while (< line to-line)
	(backslashify-current-line (> arg 0))
	(next-line 1) (setq line (1+ line))))))

(defun backslashify-current-line (doit)
  (end-of-line 1)
  (cond
   (doit
    ;; Note that "\\\\" is needed to get one backslash.
    (if (not (save-excursion (forward-char -1) (looking-at "\\\\")))
	(progn
	  (if (>= (current-column) lpc-default-macroize-column)
	      (insert " \\")
	    (while (<= (current-column) lpc-default-macroize-column)
	      (insert "\t") (end-of-line))
	    (delete-char -1)
	    (while (< (current-column) lpc-default-macroize-column)
	      (insert " ") (end-of-line))
	    (insert "\\")))))
   (t
    (forward-char -1)
    (if (looking-at "\\\\")
	(progn (skip-chars-backward " \t")
	       (kill-line))))))

;;; Below are two regular expressions that attempt to match defuns
;;; "strongly" and "weakly."  The strong one almost reconstructs the
;;; grammar of LPC; the weak one just figures anything id or curly on
;;; the left begins a defun.  The constant "lpc-match-header-strongly"
;;; determines which to use; the default is the weak one.

(defvar lpc-match-header-strongly nil
  "*If NIL, use lpc-defun-header-weak to identify beginning of definitions,\
if nonNIL, use lpc-defun-header-strong")

(defvar lpc-defun-header-strong-struct-equivs "\\(class\\|struct\\|enum\\)"
  "Regexp to match names of structure declaration blocks in LPC")

(defconst lpc-defun-header-strong
  (let*
      (; valid identifiers
       ;; There's a real wierdness here -- if I switch the below
       (id "\\(\\w\\|_\\)+")
       ;; to be
       ;; (id "\\(_\\|\\w\\)+")
       ;; things no longer work right.  Try it and see!

       ; overloadable operators
       (op-sym1
	 "[---+*/%^&|~!=<>]\\|[---+*/%^&|<>=!]=\\|<<=?\\|>>=?")
       (op-sym2
	 "&&\\|||\\|\\+\\+\\|--\\|()\\|\\[\\]")	 
       (op-sym (concat "\\(" op-sym1 "\\|" op-sym2 "\\)"))
       ; whitespace
       (middle "[^\\*]*\\(\\*+[^/\\*][^\\*]*\\)*")
       (c-comment (concat "/\\*" middle "\\*+/"))
       (wh (concat "\\(\\s \\|\n\\|//.*$\\|" c-comment "\\)"))
       (wh-opt (concat wh "*"))
       (wh-nec (concat wh "+"))
       (oper (concat "\\(" "operator" "\\("
		     wh-opt op-sym "\\|" wh-nec id "\\)" "\\)"))
       (dcl-list "([^():]*)")
       (func-name (concat "\\(" oper "\\|" id "::" id "\\|" id "\\)"))
       (inits
	 (concat "\\(:"
		 "\\(" wh-opt id "(.*\\()" wh-opt "," "\\)\\)*"
		 wh-opt id "(.*)" wh-opt "{"
		 "\\|" wh-opt "{\\)"))
       (type-name (concat
		    "\\(" lpc-defun-header-strong-struct-equivs wh-nec "\\)?"
		    id))
       (type (concat "\\(const" wh-nec "\\)?"
		     "\\(" type-name "\\|" type-name wh-opt "\\*+" "\\|"
		     type-name wh-opt "&" "\\)"))
       (modifier "\\(inline\\|virtual\\|overload\\|auto\\|static\\)")
       (modifiers (concat "\\(" modifier wh-nec "\\)*"))
       (func-header
	 ;;     type               arg-dcl
	 (concat modifiers type wh-nec func-name wh-opt dcl-list wh-opt inits))
       (inherit (concat "\\(:" wh-opt "\\(public\\|private\\)?"
			wh-nec id "\\)"))
       (cs-header (concat
		    lpc-defun-header-strong-struct-equivs
		    wh-nec id wh-opt inherit "?" wh-opt "{"))
       )
    (concat "^\\(" func-header "\\|" cs-header "\\)"))
  "Strongly-defined regexp to match beginning of structure \
or function definition.  ")


;; This part has to do with recognizing defuns.

;; The weak convention we will use is that a defun begins any time
;; there is a left curly brace, or some identifier on the left margin,
;; followed by a left curly somewhere on the line.  (This will also
;; incorrectly match some continued strings, but this is after all
;; just a weak heuristic.)  Suggestions for improvement (short of the
;; strong scheme shown above) are welcomed.

(defconst lpc-defun-header-weak "^{\\|^[_a-zA-Z].*{"
  "Weakly-defined regexp to match beginning of structure \
or function definition.  ")


(defun lpc-beginning-of-defun (arg)
  (interactive "p")
  (let ((lpc-defun-header (if lpc-match-header-strongly
			      lpc-defun-header-strong
			    lpc-defun-header-weak)))
    (cond ((or (= arg 0) (and (> arg 0) (bobp))) nil)
	  ((and (not (looking-at lpc-defun-header))
		(let ((curr-pos (point))
		      (open-pos (if (search-forward "{" nil 'move)
				    (point)))
		      (beg-pos
			(if (re-search-backward lpc-defun-header nil 'move)
			    (match-beginning 0))))
		  (if (and open-pos beg-pos
			   (< beg-pos curr-pos)
			   (> open-pos curr-pos))
		      (progn
			(goto-char beg-pos)
			(if (= arg 1) t nil));; Are we done?
		    (goto-char curr-pos)
		    nil))))
	  (t
	    (if (and (looking-at lpc-defun-header) (not (bobp)))
		(forward-char (if (< arg 0) 1 -1)))
	    (and (re-search-backward lpc-defun-header nil 'move (or arg 1))
		 (goto-char (match-beginning 0)))))))


(defun lpc-end-of-defun (arg)
  (interactive "p")
  (let ((lpc-defun-header (if lpc-match-header-strongly
			      lpc-defun-header-strong
			    lpc-defun-header-weak)))
    (if (and (eobp) (> arg 0))
	nil
      (if (and (> arg 0) (looking-at lpc-defun-header)) (forward-char 1))
      (let ((pos (point)))
	(lpc-beginning-of-defun 
	  (if (< arg 0)
	      (- (- arg (if (eobp) 0 1)))
	    arg))
	(if (and (< arg 0) (bobp))
	    t
	  (if (re-search-forward lpc-defun-header nil 'move)
	      (progn (forward-char -1)
		     (forward-sexp)
		     (beginning-of-line 2)))
	  (if (and (= pos (point)) 
		   (re-search-forward lpc-defun-header nil 'move))
	      (lpc-end-of-defun 1))))
      t
      ))
  )

(defun lpc-indent-defun ()
  "Indents the current function def, struct or class decl."
  (interactive)
  (let ((restore (point)))
    (lpc-end-of-defun 1)
    (beginning-of-line 1)
    (let ((end (point)))
      (lpc-beginning-of-defun 1)
      (while (<= (point) end)
	(lpc-indent-line)
	(next-line 1)
	(beginning-of-line 1)))
    (goto-char restore)))

(if (featurep 'hilit19)
    (progn
      (let ((comments     '(("/\\*" "\\*/" comment)))
	    (c++-comments '(("//.*$" nil comment)
			    ("^/.*$" nil comment)))
	    (strings      '((hilit-string-find ?' string)))
	    (preprocessor '(("^#[ \t]*\\(undef\\|define\\).*$" "[^\\]$" define)
			    ("^#.*$" nil include))))
	(hilit-set-mode-patterns
	 'lpc-mode
	 (append
	  comments c++-comments strings preprocessor
	  '(
	    ;; LPC datatypes
	    ("[ \n\t({]\\(\\(static\\|varargs\\)\\s +\\)*\\(\\(\\w\\|[$_]\\)+_t\\|mixed\\|float\\|void\\|int\\|string\\|mapping\\|object\\|function\\|closure\\|buffer\\|list\\([ \t]+\\(\\w\\|[$_]\\)*\\)\\)\\(\\s +\\*+)?\\|[ \n\t;()]\\)" nil type)
	    ;; LPC key words
	    ("[^_]\\<\\(return\\|goto\\|if\\|else\\|case\\|default\\|switch\\|break\\|continue\\|while\\|do\\|for\\|foreach\\|public\\|private\\|inherit\\|lambda\\)\\>[^_]"
	     1 keyword)
	    ;; MudOS efuns
	    ("[^_]\\<\\(acos\\|add_action\\|all_inventory\\|allocate\\|allocate_buffer\\|allocate_mapping\\|asin\\|atan\\|author_stats\\|break_string\\|bufferp\\|cache_stats\\|call_other\\|call_out\\|call_out_info\\|capitalize\\|catch\\|ceil\\|children\\|clear_bit\\|clone_object\\|clonep\\|command\\|commands\\|cos\\|cp\\|crc32\\|crypt\\|ctime\\|debug_info\\|debugmalloc\\|deep_inherit_list\\|deep_inventory\\|destruct\\|disable_commands\\|disable_wizard\\|domain_stats\\|dump_file_descriptors\\|dump_prog\\|dump_socket_status\\|dumpallobj\\|each\\|ed\\|enable_commands\\|enable_wizard\\|environment\\|error\\|errorp\\|exec\\|exp\\|explode\\|export_uid\\|file_name\\|file_size\\|filter_array\\|find_call_out\\|find_living\\|find_object\\|find_player\\|first_inventory\\|floatp\\|floor\\|function_exists\\|function_profile\\|functionp\\|generate_source\\|get_char\\|get_config\\|get_dir\\|geteuid\\|getuid\\|implode\\|in_edit\\|in_input\\|inherit_list\\|inherits\\|input_to\\|interactive\\|intp\\|keys\\|link\\|living\\|livings\\|localtime\\|log\\|lower_case\\|malloc_status\\|map_array\\|map_delete\\|mapp\\|master\\|match_path\\|member_array\\|memory_info\\|message\\|mkdir\\|moncontrol\\|move_object\\|mud_status\\|new\\|next_inventory\\|notify_fail\\|nullp\\|objectp\\|objects\\|opcprof\\|origin\\|parse_command\\|pointerp\\|pow\\|present\\|previous_object\\|printf\\|process_string\\|process_value\\|query_heart_beat\\|query_host_name\\|query_idle\\|query_ip_name\\|query_ip_number\\|query_load_average\\|query_privs\\|query_shadowing\\|query_snoop\\|query_snooping\\|query_verb\\|random\\|read_buffer\\|read_bytes\\|read_file\\|receive\\|reclaim_objects\\|refs\\|regexp\\|reload_object\\|remove_action\\|remove_call_out\\|rename\\|replace_program\\|replace_string\\|resolve\\|restore_object\\|rm\\|rmdir\\|rusage\\|save_object\\|say\\|set_author\\|set_bit\\|set_debug_level\\|set_eval_limit\\|set_heart_beat\\|set_hide\\|set_light\\|set_living_name\\|set_malloc_mask\\|set_privs\\|set_reset\\|seteuid\\|shadow\\|shout\\|shutdown\\|sin\\|sizeof\\|snoop\\|socket_accept\\|socket_acquire\\|socket_address\\|socket_bind\\|socket_close\\|socket_connect\\|socket_create\\|socket_error\\|socket_listen\\|socket_release\\|socket_write\\|sort_array\\|sprintf\\|sqrt\\|sscanf\\|stat\\|strcmp\\|stringp\\|strlen\\|strsrch\\|swap\\|tail\\|tan\\|tell_object\\|tell_room\\|test_bit\\|this_object\\|this_player\\|throw\\|time\\|time_expression\\|to_float\\|to_int\\|trace\\|traceprefix\\|undefinedp\\|unique_array\\|uptime\\|userp\\|users\\|values\\|virtualp\\|wizardp\\|write\\|write_buffer\\|write_bytes\\|write_file\\)\\>[^_]"
	     1 warning)
	    ))))))

(and (featurep 'folding)
     (fold-add-to-marks-list 'lpc-mode "// {{{ " "// }}}" nil t))

(provide 'lpc-mode)
